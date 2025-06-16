#pragma once

#include <string>

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

#define OW_DEBUG 0

class OW {
public:
    enum class State {
        Idle,
        Reset,
        WaitPresence,
        PresenceDetect,

        WriteByteInit,
        WriteBitHold,
        WriteBitRelease,
        WriteBitInit,
        WriteWaitNextBit
    };

    enum class Event {
        Start,
        Timeout,
        Done,
        Error,
        BitWrite,
        ByteWrite,
        ContinueByte,
    };

#if (OW_DEBUG >= 1)
    static constexpr const char *state_names[] = {
            "Idle",
            "Reset",
            "WaitPresence",
            "PresenceDetect",

            "WriteByteInit",
            "WriteBitHold",
            "WriteBitRelease",
            "WriteBitInit",
            "WriteWaitNextBit"
    };

    static constexpr const char *event_names[] = {
            "Start",
            "Timeout",
            "Done",
            "Error",
            "BitWrite",
            "ByteWrite",
            "ContinueByte",
    };
#endif

    enum class CommandType {
        Reset,
        WriteByte,
        ReadByte
    };

    struct Transition {
        State current;
        Event event;
        State next;

        void (OW::*action)();
    };

    TIM_HandleTypeDef *getTimerHandle() const;

    State getState() const;

    explicit OW(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim);

    void init();

    /**
     * @defgroup
     */
    void handleEvent(OW::Event e);

    void writeByte(uint8_t byte);

protected:
    bool m_presence;

    virtual void resetDone(bool presenceDetected) = 0;

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

    void sendReset();

    void waitPresence();

    void finishReset();

    void startWriteByte();

    void continueWriteBit();

    void startBitHold();

    void releaseLine();

    void nextWriteBit();

    static constexpr Transition m_transitions[] = {
            /// Reset
            {State::Idle,             Event::Start,     State::Reset,            &OW::sendReset},
            {State::Reset,            Event::Timeout,   State::WaitPresence,     &OW::waitPresence},
            {State::WaitPresence,     Event::Timeout,   State::PresenceDetect,   &OW::finishReset},
            {State::PresenceDetect,   Event::Done,      State::Idle,             nullptr},
            /// WriteByte
            {State::Idle,             Event::ByteWrite, State::WriteByteInit,    &OW::startWriteByte},
            {State::WriteByteInit,    Event::Timeout,   State::WriteBitInit,     &OW::continueWriteBit},
            {State::WriteBitInit,     Event::Timeout,   State::WriteBitHold,     &OW::startBitHold},
            {State::WriteBitHold,     Event::Timeout,   State::WriteBitRelease,  &OW::releaseLine},
            {State::WriteBitRelease,  Event::Timeout,   State::WriteWaitNextBit, &OW::nextWriteBit},
            {State::WriteWaitNextBit, Event::Timeout,   State::WriteBitInit,     &OW::continueWriteBit},
    };
};

class DS18X20 : public OW {
public:
    DS18X20(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim)
            : OW(port, pin, htim) {}

protected:
    void resetDone(bool presence) override {
        print_log(DEBUG_LOG, presence ? "Presence detected\r\n" : "No presence\r\n");
        handleEvent(Event::Done);
    }
};