#include "OW.hpp"

OW::OW(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim) :
        m_port(port),
        m_pin(pin),
        m_htim(htim),
        m_state(State::Idle),
        m_presence(false) {
}

void OW::init() {

}

void OW::startReset() {
    if (m_state != State::Idle) return;

    m_state = State::Reset_Pulse;
    setLineOutput();
    writeLine(false);
    startTimerUs(480);
}

OW::State OW::getState() const {
    return m_state;
}

void OW::setLineOutput() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = m_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(m_port, &GPIO_InitStruct);
}

void OW::setLineInput() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = m_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(m_port, &GPIO_InitStruct);
}

void OW::writeLine(bool level) {
    HAL_GPIO_WritePin(m_port, m_pin, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool OW::readLine() {
    return HAL_GPIO_ReadPin(m_port, m_pin) == GPIO_PIN_SET;
}

TIM_HandleTypeDef *OW::getTimerHandle() const {
    return m_htim;
}

void OW::startTimerUs(uint32_t us) {
    __HAL_TIM_DISABLE(m_htim);
    __HAL_TIM_SET_COUNTER(m_htim, 0);
    __HAL_TIM_SET_AUTORELOAD(m_htim, us - 1);
    __HAL_TIM_CLEAR_FLAG(m_htim, TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(m_htim, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE(m_htim);
}

void OW::stoplTimer() {
    __HAL_TIM_DISABLE_IT(m_htim, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(m_htim);
}

void OW::handleEvent(OW::Event e) {
    switch (m_state) {
        case State::Reset_Pulse:
            if (e == Event::Timeout) {
                writeLine(true);
                setLineInput();
                m_state = State::Wait_Presence;
                startTimerUs(70);
            }
            break;

        case State::Wait_Presence:
            if (e == Event::Timeout) {
                m_state = State::Presence_Detect;
                m_presence = !readLine();
                m_state = m_presence ? State::Done : State::Error;
                stoplTimer();
                onDone(m_presence);
            }
            break;

        default:
            break;
    }
}

void OW::onTimerElapsed() {
    handleEvent(Event::Timeout);
}

void OW::resetState() {
    if (m_state == State::Done)
        m_state = State::Idle;
}
