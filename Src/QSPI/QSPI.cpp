#include "QSPI.hpp"

void QSPI::Init() {
    BSP_QSPI_Init();
    BSP_QSPI_EnableMemoryMappedMode();
}

void QSPI::getStatus() {

}

QSPI_Info QSPI::getInfo() {
    QSPI_Info info;
    BSP_QSPI_GetInfo(&info);
    return info;
}

void QSPI::Reset() {

}
