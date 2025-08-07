#include "FreeRTOS.h"
#include "semphr.h"
#include "uart_log.h"

SemaphoreHandle_t twiSemaphore = nullptr;
static StaticSemaphore_t twiSemaphoreBuffer;

void SemphrsInit(){
    twiSemaphore = xSemaphoreCreateBinaryStatic(&twiSemaphoreBuffer);

    if (twiSemaphore == nullptr) {
        print_log(ERROR_LOG, "Error creating twiSemaphore\n\r");
    }
}