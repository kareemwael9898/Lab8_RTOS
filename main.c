/*
 * main.c — Temperature-Based Motor Control with FreeRTOS
 *
 * ATmega168  |  FreeRTOS
 *
 * Tasks:
 *   vButtonTask      (priority 2) — polls push-button, toggles override mode
 *   vTemperatureTask (priority 1) — reads LM35, decides motor state
 *   vMotorTask       (priority 1) — receives motor commands via queue
 *
 * Shared resources:
 *   motorQueue  — uint8_t commands (MOTOR_CMD_STOP / MOTOR_CMD_FORWARD)
 *   uartMutex   — protects UART from garbled output
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* HAL / MCAL drivers */
#include "uart.h"
#include "lm35.h"
#include "motor.h"
#include "button.h"
#include "lcd.h"

/* -------- Motor command codes -------- */
#define MOTOR_CMD_STOP     0
#define MOTOR_CMD_FORWARD  1

/* -------- Shared FreeRTOS objects -------- */
static QueueHandle_t      motorQueue  = NULL;
static SemaphoreHandle_t  uartMutex   = NULL;

/* -------- Override flag -------- */
static volatile uint8_t override_mode = 0;   /* 1 = button forces motor OFF */

/* ==========================================================
 *  TASK 1 — Button   (HIGH priority — preempts temp task)
 * ========================================================== */
void vButtonTask(void *pv)
{
    uint8_t prev_state = 0;
    uint8_t curr_state = 0;
    uint8_t motor_cmd;

    while (1)
    {
        curr_state = BUTTON_read();   /* 1 = pressed (active-low HW) */

        /* Detect rising edge (press) */
        if (curr_state && !prev_state)
        {
            /* Toggle override mode */
            override_mode ^= 1;

            if (xSemaphoreTake(uartMutex, portMAX_DELAY) == pdTRUE)
            {
                if (override_mode)
                {
                    UART_sendString("[BTN] Override ON  — motor forced OFF\r\n");
                }
                else
                {
                    UART_sendString("[BTN] Override OFF — sensor in control\r\n");
                }
                xSemaphoreGive(uartMutex);
            }

            /* Immediately update motor state based on mode */
            if (override_mode)
            {
                motor_cmd = MOTOR_CMD_STOP;
                xQueueOverwrite(motorQueue, &motor_cmd);
            }
            else
            {
                /* Force immediate evaluation of temperature to return control instantly */
                uint8_t temp = LM35_read();
                motor_cmd = (temp > TEMP_THRESHOLD) ? MOTOR_CMD_FORWARD : MOTOR_CMD_STOP;
                xQueueOverwrite(motorQueue, &motor_cmd);
            }
        }

        prev_state = curr_state;

        /* 50 ms polling = simple debounce */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* ==========================================================
 *  TASK 2 — Temperature   (NORMAL priority)
 * ========================================================== */
void vTemperatureTask(void *pv)
{
    uint8_t temperature;
    uint8_t motor_cmd;

    while (1)
    {
        temperature = LM35_read();

        /* Print the current temperature (mutex-protected) */
        if (xSemaphoreTake(uartMutex, portMAX_DELAY) == pdTRUE)
        {
            UART_sendString("[TEMP] ");
            uart_print_number(temperature);
            UART_sendString(" C");

            LCD_setCursor(0, 0);
            LCD_sendString("Temp: ");
            LCD_printNumber(temperature);

            if (override_mode)
            {
                UART_sendString("  (override active)");
                LCD_sendString("C OVR");
            }
            else
            {
                LCD_sendString("C    ");
            }
            UART_sendString("\r\n");
            xSemaphoreGive(uartMutex);
        }

        /* Only send motor commands when NOT in override mode */
        if (!override_mode)
        {
            if (temperature > TEMP_THRESHOLD)
            {
                motor_cmd = MOTOR_CMD_FORWARD;
            }
            else
            {
                motor_cmd = MOTOR_CMD_STOP;
            }
            xQueueOverwrite(motorQueue, &motor_cmd);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ==========================================================
 *  TASK 3 — Motor   (NORMAL priority)
 * ========================================================== */
void vMotorTask(void *pv)
{
    uint8_t motor_cmd;
    uint8_t last_state = 0xFF;   /* force first print */

    while (1)
    {
        /* Block until a command arrives */
        if (xQueueReceive(motorQueue, &motor_cmd, portMAX_DELAY) == pdTRUE)
        {
            /* Only act and print when the state actually changes */
            if (motor_cmd != last_state)
            {
                if (motor_cmd == MOTOR_CMD_FORWARD)
                {
                    motor_forward();
                }
                else
                {
                    motor_stop();
                }

                if (xSemaphoreTake(uartMutex, portMAX_DELAY) == pdTRUE)
                {
                    LCD_setCursor(1, 0);
                    if (motor_cmd == MOTOR_CMD_FORWARD)
                    {
                        UART_sendString("[MOTOR] Running FORWARD\r\n");
                        LCD_sendString("Motor: FORWARD  ");
                    }
                    else
                    {
                        UART_sendString("[MOTOR] STOPPED\r\n");
                        LCD_sendString("Motor: STOPPED  ");
                    }
                    xSemaphoreGive(uartMutex);
                }

                last_state = motor_cmd;
            }
        }
    }
}

/* ==========================================================
 *  MAIN
 * ========================================================== */
int main(void)
{
    /* ---- Peripheral init ---- */
    UART_init(9600);
    LM35_init();
    motor_init();
    BUTTON_init();
    LCD_init();

    UART_sendString("=== FreeRTOS Motor Control ===\r\n");

    /* ---- Create FreeRTOS objects ---- */
    motorQueue = xQueueCreate(1, sizeof(uint8_t));
    uartMutex  = xSemaphoreCreateMutex();

    /* ---- Create tasks ---- */
    xTaskCreate(vButtonTask,      "BTN",  80, NULL, 2, NULL);
    xTaskCreate(vTemperatureTask, "TEMP", 80, NULL, 1, NULL);
    xTaskCreate(vMotorTask,       "MOT",  80, NULL, 1, NULL);

    /* ---- Start scheduler (never returns) ---- */
    vTaskStartScheduler();

    while (1);
}