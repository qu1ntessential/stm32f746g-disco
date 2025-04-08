#ifndef I2C_HPP
#define I2C_HPP

#include <functional>

#include "stm32f7xx.h"
#include "uart_log.h"

#include "FreeRTOS.h"
#include "semphr.h"

class I2C {
    I2C_HandleTypeDef *m_hi2c;

    static const uint8_t slaveAddr = 0x90;

    static uint8_t crc8(uint8_t *buffer, uint8_t length);

    static inline void Error_Handler() {
        print_log(ERROR_LOG, "I2C error handler called!\r\n");
    }

    void MasterTxCpltCallback();

    void MasterRxCpltCallback();

    void ErrorCallback();

    // ������� ��� callback-�������
    static void MasterTxCpltCallbackWrapper(I2C_HandleTypeDef *hi2c) {
        I2C *twi = reinterpret_cast<I2C *>(hi2c->pBuffPtr);
        if (twi) {
            twi->MasterTxCpltCallback();
        }
    }

    static void MasterRxCpltCallbackWrapper(I2C_HandleTypeDef *hi2c) {
        I2C *twi = reinterpret_cast<I2C *>(hi2c->pBuffPtr);
        if (twi) {
            twi->MasterRxCpltCallback();
        }
    }

    static void ErrorCallbackWrapper(I2C_HandleTypeDef *hi2c) {
        I2C *twi = reinterpret_cast<I2C *>(hi2c->pBuffPtr);
        if (twi) {
            twi->ErrorCallback();
        }
    }

    inline HAL_StatusTypeDef
    MasterTransmitIT(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout = 100) {
        return HAL_I2C_Master_Transmit_IT(m_hi2c, DevAddress, pData, Size);
    }

    inline HAL_StatusTypeDef
    MasterReceiveIT(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout = 100) {
        return HAL_I2C_Master_Receive_IT(m_hi2c, DevAddress, pData, Size);
    }

    inline HAL_StatusTypeDef
    MemWrite(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size,
             uint32_t Timeout = 100) {
        return HAL_I2C_Mem_Write(m_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
    }

    inline HAL_StatusTypeDef
    MemRead(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size,
            uint32_t Timeout = 100) {
        return HAL_I2C_Mem_Read(m_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
    }

    inline HAL_StatusTypeDef
    MemWriteDMA(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
        return HAL_I2C_Mem_Write_DMA(m_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    }

    inline HAL_StatusTypeDef
    MemReadDMA(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size) {
        return HAL_I2C_Mem_Read_DMA(m_hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
    }

    inline HAL_StatusTypeDef
    MasterSeqTransmitIT(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions = I2C_FIRST_FRAME) {
        return HAL_I2C_Master_Seq_Transmit_IT(m_hi2c, DevAddress, pData, Size, XferOptions);
    }

    inline HAL_StatusTypeDef
    MasterSeqReceiveIT(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions = I2C_LAST_FRAME) {
        return HAL_I2C_Master_Seq_Receive_IT(m_hi2c, DevAddress, pData, Size, XferOptions);
    }

    inline HAL_StatusTypeDef
    MasterSeqTransmitDMA(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions = I2C_FIRST_FRAME) {
        return HAL_I2C_Master_Seq_Transmit_DMA(m_hi2c, DevAddress, pData, Size, XferOptions);
    }

    inline HAL_StatusTypeDef
    MasterSeqReceiveDMA(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions = I2C_LAST_FRAME) {
        return HAL_I2C_Master_Seq_Receive_DMA(m_hi2c, DevAddress, pData, Size, XferOptions);
    }

    inline HAL_I2C_StateTypeDef GetState() {
        return m_hi2c->State;
    }

    inline HAL_StatusTypeDef CheckDevice(uint16_t devAddr, uint32_t trials = 5) {
        return HAL_I2C_IsDeviceReady(m_hi2c, devAddr, trials, 100);
    }

public:
    explicit I2C(I2C_HandleTypeDef *hi2c) : m_hi2c(hi2c) {
        if (m_hi2c == nullptr) {
            print_log(ERROR_LOG, "Received nullptr!\r\n");
        }
    };

    enum Orders {
        GET_VERSION = 1,      ///< 2 bytes (version number)
        SET_MONO_CUT_PWR,     ///< 2 bytes (the Powers struct)
        SET_MONO_MIX_PWR,
        SET_MONO_COAG_PWR,
        SET_BI_CUT_PWR,
        SET_BI_MIX_PWR,
        SET_BI_COAG_PWR,
        SET_TIMEOUT,          ///< 2 bytes (in seconds)
        GET_STATE,            ///< 2 byte (States struct)
        GET_RESET_CODE,       ///< 1 byte (MCUCSR value)
        ORDER_END
    };

    void Init() {
        HAL_I2C_RegisterCallback(m_hi2c,
                                 HAL_I2C_MASTER_TX_COMPLETE_CB_ID,
                                 &I2C::MasterTxCpltCallbackWrapper);
        HAL_I2C_RegisterCallback(m_hi2c,
                                 HAL_I2C_MASTER_RX_COMPLETE_CB_ID,
                                 &I2C::MasterRxCpltCallbackWrapper);
        HAL_I2C_RegisterCallback(m_hi2c,
                                 HAL_I2C_ERROR_CB_ID,
                                 &I2C::ErrorCallbackWrapper);

        m_hi2c->pBuffPtr = reinterpret_cast<uint8_t *>(this);
    }

    uint8_t BusScan(uint8_t *addr_array_ptr);

    bool putData(Orders order, uint16_t value, uint8_t addr = slaveAddr);

    bool getData(Orders order, uint8_t *value, uint8_t *length, uint8_t addr = slaveAddr);
};

#endif //I2C_HPP
