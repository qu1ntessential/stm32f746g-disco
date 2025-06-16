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
}

void OW::sendReset() {
    setLineOutput();
    writeLine(false);
    startTimerUs(480);
}

void OW::waitPresence() {
    stopTimer();
    writeLine(true);
    startTimerUs(40); ///< Более правильно читать линию по прерыванию
}

void OW::finishReset() {
    stopTimer();
    setLineInput();
    m_presence = !readLine();
    resetDone(m_presence);
}

/// Idle -> WriteBitInit -> WriteBitHold -> WriteBitRelease

void OW::writeByte(uint8_t byte) {
    if (m_state != State::Idle) return;

    m_currentByte = byte;
    m_byteBitIndex = 0;
    m_currentBit = (m_currentByte >> m_byteBitIndex) & 0x01;

    handleEvent(Event::ByteWrite);
}

void OW::startWriteByte() {
    m_byteBitIndex = 0;
    m_currentBit = (m_currentByte >> m_byteBitIndex) & 0x01;
    handleEvent(Event::Timeout);
}

void OW::continueWriteBit() {
    stopTimer();

    setLineOutput();
    writeLine(false);

    startTimerUs(2);
}

void OW::startBitHold() {
    stopTimer();

    if (m_currentBit) {

        writeLine(true);
        setLineInput();
        startTimerUs(60);
    } else {

        startTimerUs(60);
    }
}

void OW::releaseLine() {
    stopTimer();

    if (!m_currentBit) {
        writeLine(true);
        setLineInput();
    }

    startTimerUs(5);
}

void OW::nextWriteBit() {
    stopTimer();

    m_byteBitIndex++;
    if (m_byteBitIndex < 8) {
        m_currentBit = (m_currentByte >> m_byteBitIndex) & 0x01;
        handleEvent(Event::Timeout);
    } else {
        m_state = State::Idle;
        resetDone(true);
    }
}

