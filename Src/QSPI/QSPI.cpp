#include "QSPI.hpp"

void QSPI::Init() {
    BSP_QSPI_Init();
    BSP_QSPI_EnableMemoryMappedMode();
}

void QSPI::getStatus() {

}

void QSPI::getInfo() {
    QSPI_Info info;
    BSP_QSPI_GetInfo(&info);
    print_log(ERROR_LOG, "FlashSize: %d", info.FlashSize);
}

void QSPI::Reset() {

}
