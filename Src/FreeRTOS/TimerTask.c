#include "FreeRTOS.h"

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @brief Размер стека задачи таймеров (в словах)
 */
#define TIMER_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)

/**
 * @brief Предоставляет FreeRTOS статически выделенные память и стек
 * для внутренней таймерной задачи (Timer Service Task).
 *
 * Эта функция вызывается ядром FreeRTOS только при включённой статической аллокации
 * и наличии включённой поддержки таймеров (configUSE_TIMERS == 1).
 *
 * Здесь выделяются:
 *  - StaticTask_t: структура управления задачей (TCB)
 *  - StackType_t[]: стек задачи
 *
 * Память должна быть доступна всё время жизни задачи — обычно выделяется как `static`.
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *puxTimerTaskStackSize) {
    /// Статический буфер для TCB (Task Control Block)
    static StaticTask_t xTimerTaskTCB;

    /// Статический стек для задачи таймеров
    static StackType_t uxTimerTaskStack[TIMER_TASK_STACK_SIZE];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *puxTimerTaskStackSize = TIMER_TASK_STACK_SIZE;
}

#endif /* #if (configSUPPORT_STATIC_ALLOCATION == 1) */
