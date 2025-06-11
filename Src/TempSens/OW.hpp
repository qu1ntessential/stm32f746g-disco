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

#define OW_DEBUG 1

class OW {
public:
    enum class State {
        Idle,
        Reset,
        WaitPresence,
        PresenceDetect,
        Done,
        Error,
    };

    enum class Event {
        Start,
        Timeout,
    };

#if (OW_DEBUG == 1)
    static constexpr const char *state_names[] = {
            "Idle",
            "Reset",
            "WaitPresence",
            "PresenceDetect",
            "Done",
            "Error",
    };

    static constexpr const char *event_names[] = {
            "Start",
            "Timeout",
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

protected:
    bool m_presence;

    virtual void onDone(bool presenceDetected) = 0;

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

    static constexpr Transition m_transitions[] = {
            {State::Idle,  Event::Start,   State::Reset,        &OW::sendReset},
            {State::Reset, Event::Timeout, State::WaitPresence, &OW::waitPresence},
            {State::WaitPresence, Event::Timeout, State::PresenceDetect, &OW::finishReset}
    };
};

class OWTester : public OW {
public:
    OWTester(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim)
            : OW(port, pin, htim) {}

protected:
    void onDone(bool presence) override {
        print_log(DEBUG_LOG, presence ? "Presence detected\n" : "No presence\n");
    }
};