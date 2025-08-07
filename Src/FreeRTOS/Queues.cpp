#include "FreeRTOS.h"
#include "queue.h"
#include "uart_log.h"

#define UART_QUEUE_LENGTH 512
#define UART_QUEUE_ITEM_SIZE sizeof(char)

QueueHandle_t uartQueue = nullptr;
static StaticQueue_t xUartQueueStruct;
static uint8_t ucUartQueueStorageArea[UART_QUEUE_LENGTH * UART_QUEUE_ITEM_SIZE];


void QueuesInit() {
    uartQueue = xQueueCreateStatic(UART_QUEUE_LENGTH,
                                   UART_QUEUE_ITEM_SIZE,
                                   ucUartQueueStorageArea,
                                   &xUartQueueStruct);

    if (uartQueue == nullptr) {
        print_log(ERROR_LOG, "Error creating uiCmdQueue\n\r");
    } else {
        vQueueAddToRegistry(uartQueue, "uartQueue");
    }
}

extern "C" int __io_putchar(int ch) {
    if (uartQueue == NULL) return EOF;

    auto c = (uint8_t) ch;
    BaseType_t status;

    if (xPortIsInsideInterrupt()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        status = xQueueSendFromISR(uartQueue, &c, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        status = xQueueSend(uartQueue, &c, pdMS_TO_TICKS(10));
    }

    return (status == pdTRUE) ? ch : EOF;
}