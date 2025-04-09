#ifndef QSPI_IC_HPP
#define QSPI_IC_HPP

#include "stm32f7xx.h"
#include "stm32746g_discovery_qspi.h"

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

    void getStatus();

    void Reset();
};

#endif //QSPI_IC_HPP
