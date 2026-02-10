#include "QSPI.hpp"

void QSPI::Init() {
    BSP_QSPI_Init();
    BSP_QSPI_EnableMemoryMappedMode();
}

void QSPI::getStatus() {

}

QSPI_Info QSPI::getInfo() const {
    QSPI_Info info;
    BSP_QSPI_GetInfo(&info);
    return info;
}

void QSPI::Reset() {

}

void QSPI::Read() {
    //HAL_QSPI_Transmit_DMA(m_QSPIHandle, &data);
}

void QSPI::Write() {

}
