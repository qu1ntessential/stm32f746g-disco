#include "OW.hpp"


OW::OW(GPIO_TypeDef *port, uint16_t pin, TIM_HandleTypeDef *htim) :
        m_presence(false),
        m_port(port),
        m_pin(pin),
        m_htim(htim),
        m_state(State::Idle),
        m_readBit(false),
        m_currentBit(false),
        m_currentByte(0),
        m_byteBitIndex(0) {}

void OW::init() {
    setLineInput();
    m_state = State::Idle;
}

void OW::sendReset() {
    m_state = State::Reset;
    setLineOutput();
    writeLine(false);
    startTimerUs(480);
}

void OW::waitPresence() {
    m_state = State::WaitPresence;
    writeLine(true);
    setLineInput();
    startTimerUs(70);
}

void OW::finishReset() {
    m_state = State::PresenceDetect;
    stopTimer();
    m_presence = !readLine();
    onDone(m_presence);
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

void OW::stopTimer() {
    __HAL_TIM_DISABLE_IT(m_htim, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(m_htim);
}

void OW::handleEvent(OW::Event e) {
    for (const auto &t: m_transitions) {
        if (t.current == m_state && t.event == e) {
            m_state = t.next;
            if (t.action)
                (this->*t.action)();
            return;
        }
    }

#if (OW_DEBUG == 1)
    print_log(ERROR_LOG,
              "FSM: No transition for state=%s, event=%s\r\n",
              state_names[static_cast<int>(m_state)],
              event_names[static_cast<int>(e)]);
#endif
}
