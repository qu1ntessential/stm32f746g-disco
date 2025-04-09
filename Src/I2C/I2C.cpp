#include "I2C.hpp"

#include <cstring>

extern SemaphoreHandle_t twiSemaphore; ///<

/**
 * @brief Алгоритм вычисления контольной суммы CRC8
 * @param buffer Указатель на пакет данных для расчета CRC
 * @param length Количество данных в пакете
 * @return Значение контрольной суммы
 */
uint8_t I2C::crc8(uint8_t *buffer, uint8_t length) {
    uint8_t crc = 0x82;

    while (length--) {
        crc ^= *buffer++;

        for (uint8_t i = 0; i < 8; i++)
            crc = (crc & 1) ? (crc >> 1) ^ 0x8C : crc >> 1;
    }
    return crc;
}

uint8_t I2C::BusScan(uint8_t *addr_array_ptr) {
    print_log(DEBUG_LOG, "Scanning I2C bus (finding device's 7-bit address)...\r\n");
    uint8_t cnt = 0;
    for (uint8_t address = 0x03; address <= 0x7F; address++) {
        uint8_t i2cAddress = address << 1;
        if (CheckDevice(i2cAddress) == HAL_OK) {
            addr_array_ptr[cnt] = address;
            print_log(DEBUG_LOG, "Device found at address: 0x%02X\r\n", address);
            cnt++;
        }
    }
    print_log(DEBUG_LOG, "Scan complete. %d devices found.\r\n", cnt);
    return cnt;
}

void I2C::MasterTxCpltCallback() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(twiSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void I2C::MasterRxCpltCallback() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(twiSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void I2C::ErrorCallback() {

}

/**
 * @brief Функция передачи ESG
 * @param order Команда из протокола обмена с ЭХВЧ
 * @param value Два байта параметра команды в пакете I2C
 * @param addr Slave address I2C (default address = 0x90)
 * @return True - получены корректные данные, false- произошла ошибка
 */
bool I2C::putData(Orders order, uint16_t value, uint8_t addr) {
    if (order < 2 || order > 8) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Incorrect input order\r\n");
#endif
        return false;
    }

    static uint8_t data_tx[4] = {addr,
                                 static_cast<uint8_t>(order),
                                 static_cast<uint8_t>(value & 0xFF),
                                 static_cast<uint8_t>((value >> 8) & 0xFF)};
    uint8_t crc = crc8(data_tx, sizeof(data_tx));

    data_tx[0] = data_tx[1];
    data_tx[1] = data_tx[2];
    data_tx[2] = data_tx[3];
    data_tx[3] = crc;

    if (MasterSeqTransmitIT(addr, data_tx, sizeof(data_tx)) != HAL_OK) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "MasterSeqTransmitIT error\r\n");
#endif
    }

    /// Ожидаем взятия семафора 500 мс (семафор выставляется в callback-функции завершения передачи)
    if (xSemaphoreTake(twiSemaphore, pdMS_TO_TICKS(50)) != pdPASS) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "I2C TX timeout\r\n");
#endif
        return false;
    }

    static uint8_t data_rx[4];

    if (MasterSeqReceiveIT(addr, data_rx, sizeof(data_rx) - 1) != HAL_OK) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "MasterSeqReceiveIT error\r\n");
#endif
    }

    /// Ожидаем взятия семафора 500 мс
    if (xSemaphoreTake(twiSemaphore, pdMS_TO_TICKS(500)) != pdTRUE) {
        print_log(ERROR_LOG, "I2C RX timeout\r\n");
        return false;
    }

    data_rx[3] = data_rx[2];
    data_rx[2] = data_rx[1];
    data_rx[1] = data_rx[0];
    data_rx[0] = addr;

    if (crc8(data_rx, 4) != 0) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "CRC error\r\n");
#endif
        return false;
    }

    return true;
}

/**
 * @brief Функция запроса версии или состояния ЭХВЧ с платы процессора по I2C
 * @param order Команда из протокола обмена с ЭХВЧ
 * @param value Указатель на
 * @param length
 * @param addr Slave address I2C (default address = 0x90)
 * @return True - получены корректные данные, false- произошла ошибка
 */
bool I2C::getData(Orders order, uint8_t *value, uint8_t *length, uint8_t addr) {
    if (order != GET_VERSION && order != GET_STATE) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Incorrect input order\r\n");
#endif
        return false;
    }

    static uint8_t data_tx[2];

    if (order == GET_STATE) {
        data_tx[0] = GET_STATE;
        data_tx[1] = 0xE1;
    } else {
        data_tx[0] = GET_VERSION;
        data_tx[1] = 0x23;
    }

    if (MasterSeqTransmitIT(addr, data_tx, sizeof(data_tx)) != HAL_OK) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "MasterSeqTransmitIT error\r\n");
#endif
    }

    if (xSemaphoreTake(twiSemaphore, pdMS_TO_TICKS(500)) != pdTRUE) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "I2C TX timeout\r\n");
#endif
        return false;
    }

    static uint8_t data_rx[5];

    if (MasterSeqReceiveIT(addr, data_rx, sizeof(data_rx) - 1) != HAL_OK) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "MasterSeqReceiveIT error\r\n");
#endif
    }

    if (xSemaphoreTake(twiSemaphore, pdMS_TO_TICKS(500)) != pdTRUE) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "I2C RX timeout\r\n");
#endif
        return false;
    }

    data_rx[4] = data_rx[3];
    data_rx[3] = data_rx[2];
    data_rx[2] = data_rx[1];
    data_rx[1] = data_rx[0];
    data_rx[0] = addr;

    if (crc8(data_rx, 5) != 0) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "CRC error\r\n");
#endif
        return false;
    }

    /// Проверка, что в посылке 3 байта полезных данных
    if (data_rx[1] != 3) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Received packet's length incorrect\r\n");
#endif
        return false;
    }

    memcpy(value, &data_rx[2], *length = data_rx[1] - 1);

    return true;
}
