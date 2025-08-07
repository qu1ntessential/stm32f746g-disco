#include "FreeRTOS.h"
#include "task.h"

#if (configSUPPORT_STATIC_ALLOCATION == 1)

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

/**
 * @brief Функция, предоставляющая FreeRTOS память для статического создания задачи IDLE
 * @param xIdleTaskTCBBuffer Указатель на буфер для TCB (Task Control Block)
 * @param uxIdleTaskStack Указатель на стек задачи IDLE
 * @param IdleTaskStackSize Размер стека
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   configSTACK_DEPTH_TYPE *puxIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#endif /* #if (configSUPPORT_STATIC_ALLOCATION == 1) */
