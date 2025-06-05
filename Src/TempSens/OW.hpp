#pragma once

#include "stm32f7xx.h"
#include "uart_log.h"

class OW {
public:
    enum class State {
        Idle,
        Reset_Pulse,
        Wait_Presence,
        Presence_Detect,
        Done,
        Error
    };

    enum class Event {
        Start,
        Timeout,
        LineLow,
        LineHigh,
        PresenceOk,
        PresenceFail
    };

    explicit OW(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim);

    void init();

    void startReset();

    State getState() const;

    TIM_HandleTypeDef *getTimerHandle() const;

    void onTimerElapsed();

    void resetState();

protected:
    virtual void onDone(bool presenceDetected) = 0;

    void setLineOutput();

    void setLineInput();

    void writeLine(bool level);

    bool readLine();

    void startTimerUs(uint32_t us);

    void stoplTimer();

private:
    GPIO_TypeDef *m_port;
    uint16_t m_pin;
    TIM_HandleTypeDef *m_htim;
    State m_state;
    bool m_presence;

    void handleEvent(Event e);
};

class OWTester : public OW {
public:
    OWTester(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim)
            : OW(port, pin, htim) {}

protected:
    void onDone(bool presenceDetected) override {
        if (presenceDetected) {
            print_log(DEBUG_LOG, "Device detected!\n");
        } else {
            print_log(ERROR_LOG, "No device detected.\n");
        }
    }
};