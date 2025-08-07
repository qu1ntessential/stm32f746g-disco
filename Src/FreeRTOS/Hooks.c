#include "FreeRTOS.h"
#include "task.h"

#if (configCHECK_FOR_STACK_OVERFLOW)

/**
 * @brief Функция-обработчик, вызываемая FreeRTOS при обнаружении переполнения стека в любой задаче
 * @param xTask Handle задачи, где произошло переполнение
 * @param pcTaskName Имя задачи, у которой переполнился стек
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName) {
    while (1) {}
}

#endif /* #if (configCHECK_FOR_STACK_OVERFLOW) */

#if (configUSE_MALLOC_FAILED_HOOK == 1)

/**
 * @brief
 */
void vApplicationMallocFailedHook(void) {}

#endif /* #if (configUSE_MALLOC_FAILED_HOOK == 1) */

#if (configUSE_IDLE_HOOK == 1)

/**
 * @brief
 */
void vApplicationIdleHook(void) {}

#endif /* #if (configUSE_IDLE_HOOK == 1) */

#if (configUSE_TICK_HOOK == 1)

/**
 * @brief
 */
void vApplicationTickHook(void) {}

#endif /* #if (configUSE_TICK_HOOK == 1) */
