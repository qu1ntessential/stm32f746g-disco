#pragma once

#include "stm32f7xx.h"
#include "uart_log.h"

#include "FreeRTOS.h"
#include "task.h"

/// Набор ROM-команд для работы с подчиненными устройствами
#define READ_ROM     0x33
#define MATCH_ROM    0x55
#define SKIP_ROM     0xCC
#define SEARCH_ROM   0xF0
#define ALARM_SEARCH 0xEC

extern TaskHandle_t owTaskHandle;

class OW {
public:
    enum class State {
        Idle,
        Reset_Pulse,
        Wait_Presence,
        Done,
        Error,

        WriteBit_Start,
        WriteBit_ReleaseOrHold,
        WriteBit_Done,

        ReadBit_Start,
        ReadBit_ReadSample,
        ReadBit_Done
    };

    enum class Event {
        Start,
        Timeout,
        LineLow,
        LineHigh,
        PresenceOk,
        PresenceFail
    };

    enum class CommandType {
        Reset,
        WriteByte,
        ReadByte
    };

    struct Command {
        CommandType type;
        uint8_t byte = 0; ///< Используется только для WriteByte
    };

    explicit OW(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim);

    void init();

    void startReset();

    void resetState();

    void writeBit(bool bit);

    void readBit();

    void writeByte(uint8_t byte);

    void readByteStart();

    /**
     * @defgroup Get-functions
     */
    bool getReadBit() const;

    uint8_t getReadByte() const;

    State getState() const;

    bool isDevicePresent() const { return m_presence; }

    /**
     * @defgroup
     */
    void handleEvent(Event e);

    void onTimerElapsed();

    TIM_HandleTypeDef *getTimerHandle() const;

protected:
    bool m_presence;

    virtual void onDone(bool presenceDetected) = 0;

    virtual void onWriteByteDone();

    virtual void onReadByteDone();

private:
    GPIO_TypeDef *m_port;
    uint16_t m_pin;
    TIM_HandleTypeDef *m_htim;
    State m_state;
    bool m_readBit;
    bool m_currentBit;
    uint8_t m_currentByte;
    uint8_t m_byteBitIndex;

    void setLineOutput();

    void setLineInput();

    void writeLine(bool level);

    bool readLine();

    void startTimerUs(uint32_t us);

    void stopTimer();
};

class OWTester : public OW {
public:
    OWTester(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim)
            : OW(port, pin, htim) {}

protected:
    void onWriteByteDone() override {
        print_log(DEBUG_LOG, "WriteByte done\n");
    }

    void onDone(bool presence) override {
        print_log(DEBUG_LOG, presence ? "Presence detected\n" : "No presence\n");
    }
};