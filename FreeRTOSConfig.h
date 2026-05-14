#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#define F_CPU 8000000UL   // or 16000000UL depending on your board

#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configCPU_CLOCK_HZ          ( F_CPU )
#define configTICK_RATE_HZ          ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES        4
#define configMINIMAL_STACK_SIZE    ( ( unsigned short ) 60 )
#define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 600 ) )
#define configMAX_TASK_NAME_LEN     8

#define configUSE_MUTEXES           1
#define configUSE_COUNTING_SEMAPHORES 1
#define configUSE_16_BIT_TICKS 1

#define configUSE_TIMERS            0

#define INCLUDE_vTaskDelay          1
#define INCLUDE_vTaskDelete         1
#define INCLUDE_vTaskPrioritySet    1
#define INCLUDE_uxTaskPriorityGet   1
#define INCLUDE_vTaskSuspend        1
#define INCLUDE_vTaskDelayUntil     1

#endif