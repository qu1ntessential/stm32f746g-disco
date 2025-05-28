#ifndef QSPI_IC_HPP
#define QSPI_IC_HPP

#include <cstdio>

#include "stm32f7xx.h"
#include "stm32746g_discovery_qspi.h"

#include "uart_log.h"

#define QSPI_MEM __attribute__((section(".extflash_text"), aligned(4)))

class QSPI {
    enum class Status {
        OK,
        ERROR,
        BUSY,
        NOT_SUPPORTED = 4,
        SUSPENDED = 8
    };

    QSPI_HandleTypeDef *m_QSPIHandle;
public:
    explicit QSPI(QSPI_HandleTypeDef *QSPIHandle) :
            m_QSPIHandle(QSPIHandle) {}

    void Init();

    void getStatus();

    QSPI_Info getInfo();

    void Reset();

    void Read();

    void Write();
};

#endif //QSPI_IC_HPP
