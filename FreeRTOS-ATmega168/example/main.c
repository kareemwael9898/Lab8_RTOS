/*
File: main.c
Author: Adam Rudzinski

A simple example documenting the use of FreeRTOS port for ATmega168.
(main code)

Use it as you wish, at your own risk.
*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <led.h>
#include <FreeRTOS.h>
#include <task.h>

void vMainQueueSendPassed( void )
{
}

static int uart_put(char c)
{
    if( c == '\n' )
    {
	uart_put('\r');
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static int uart_puts(const char* s)
{
    while( *s != '\0' )
    {
	uart_put(*s);
	++s;
    }
    return 0;
}

static void init_USART()
{
    UCSR0A = 0;
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

#define BAUD 2400
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~_BV(U2X0);
#endif

    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
}

static void init_sleep()
{
    PRR = _BV(PRTWI) | _BV(PRTIM2) | _BV(PRSPI) | _BV(PRADC);
    SMCR = 0;
}

static void go_to_sleep()
{
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}

static const char* to_str(uint16_t n)
{
    static char buf[] = "01234567890123456\0";
    char* i = buf + 17;
    if( n == 0 )
    {
	--i;
	*i = '0';
	return i;
    }
    do
    {
	const unsigned char v = n & 0x000f;
	n >>= 4;
	--i;
	if( v < 10 )
	{
	    *i = v + '0';
	}
	else
	{
	    *i = v - 10 + 'a';
	}
    }
    while( n );
    return i;
}

static xTaskHandle h1, h2;

#define DELAY1	10
#define DELAY2	33

static void ledtask1(void* params)
{
    LED(1, LED_ON);
    for( ;; )
    {
	wdt_reset();
	LED_toggle(1);
	uart_puts("I am Task 1!\n");
	vTaskDelay(DELAY1);
    }
    vTaskDelete(NULL);
}

static void ledtask2(void* params)
{
    LED(2, LED_ON);
    for( ;; )
    {
	wdt_reset();
	LED_toggle(2);
	uart_puts("I am Task 2!\n");
	vTaskDelay(DELAY2);
    }
    vTaskDelete(NULL);
}

__attribute__((always_inline)) static inline void show_wm(const char* const t, const xTaskHandle h)
{
    const char* s;
    UBaseType_t wm;
    uart_puts(t);
    uart_puts(" = 0x");
    wm = uxTaskGetStackHighWaterMark(h);
    s = to_str(wm);
    uart_puts(s);
}

void vApplicationIdleHook()
{
    wdt_reset();
    if( led_cnt == 0 )
    {
	const char* s;
	uart_puts("IDLE (free heap = 0x");
	s = to_str(xPortGetFreeHeapSize());
	uart_puts(s);
	uart_puts("; ");
	show_wm("w1", h1);
	uart_puts("; ");
	show_wm("w2", h2);
	uart_puts("; ");
	show_wm("wi", NULL);
	uart_puts(")\n");
	led_cnt = INIT_LED_CNT;
    }
    go_to_sleep();
}

int main()
{
    wdt_enable(WDTO_2S);
    init_sleep();
    init_USART();
    uart_puts("START\n");
    init_tmr0();
    init_LEDs();
    LED(1, LED_OFF);
    LED(2, LED_OFF);
    xTaskCreate(ledtask1, "lt1", 128, NULL, tskIDLE_PRIORITY + 1, &h1);
    xTaskCreate(ledtask2, "lt2", 128, NULL, tskIDLE_PRIORITY + 1, &h2);
    vTaskStartScheduler();
    return 1;	//should not get here
}
