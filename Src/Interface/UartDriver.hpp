#pragma once

#include "RingBuffer.hpp"

/**
 * @brief Базовый класс UartDriver, инкапсулирующий низкоуровневую работу с UART и буферами
 * @tparam TxBufSize
 * @tparam RxBufSize
 */
template<uint16_t TxBufSize = 256, uint16_t RxBufSize = 256>
class UartDriver {
public:
    UartDriver() = default;

    /// Инициализация UART — абстрактный метод, зависит от реализации
    virtual void init() = 0;

    /// Отправить один байт (поместить в txBuffer и запустить передачу)
    bool sendByte(uint8_t byte) {
        bool wasEmpty = txBuffer.isEmpty();
        if (!txBuffer.push(byte)) return false;
        if (wasEmpty) startTransmission();
        return true;
    }

    /// Отправка строки (null-terminated)
    void sendString(const char *str) {
        while (*str) {
            sendByte(static_cast<uint8_t>(*str++));
        }
    }

    /// Чтение одного байта из приёмного буфера
    bool receiveByte(uint8_t &byte) {
        return rxBuffer.pop(byte);
    }

    /// Обработчик прерываний UART (вызов из ISR)
    void irqHandler() {
        if (rxReady()) {
            uint8_t byte = readByte();
            rxBuffer.push(byte); /// Игнорируем, если буфер переполнен
        }

        if (txReady()) {
            uint8_t byte;
            if (txBuffer.pop(byte)) {
                transmitByte(byte);
            } else {
                disableTxInterrupt();
            }
        }
    }

protected:
    RingBuffer<uint8_t, TxBufSize> txBuffer;
    RingBuffer<uint8_t, RxBufSize> rxBuffer;

    /// Запуск передачи (если включена передача прерыванием)
    void startTransmission() {
        if (txReady()) {
            uint8_t byte;
            if (txBuffer.pop(byte)) {
                transmitByte(byte);
                enableTxInterrupt();
            }
        }
    }

    /// Платформозависимые методы (реализуются в наследнике)
    virtual void transmitByte(uint8_t byte) = 0;

    virtual uint8_t readByte() = 0;

    virtual bool txReady() = 0;

    virtual bool rxReady() = 0;

    virtual void enableTxInterrupt() = 0;

    virtual void disableTxInterrupt() = 0;
};