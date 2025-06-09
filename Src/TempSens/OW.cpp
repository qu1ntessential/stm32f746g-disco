#include "OW.hpp"

TaskHandle_t owTaskHandle;

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

void OW::stopTimer() {
    __HAL_TIM_DISABLE_IT(m_htim, TIM_IT_UPDATE);
    __HAL_TIM_DISABLE(m_htim);
}

void OW::handleEvent(OW::Event e) {
    print_log(DEBUG_LOG,
              "FSM: state=%d, event=%d\r\n",
              static_cast<int>(m_state),
              static_cast<int>(e));
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
                m_presence = readLine();
                m_state = m_presence ? State::Done : State::Error;
                stopTimer();
                onDone(m_presence);
            }
            break;

        case State::WriteBit_Start:
            if (e == Event::Timeout) {
                m_state = State::WriteBit_ReleaseOrHold;
                if (m_currentBit) {
                    setLineInput();
                    startTimerUs(60);
                } else {
                    startTimerUs(60);
                }
            }
            break;

        case State::WriteBit_ReleaseOrHold:
            if (e == Event::Timeout) {
                if (!m_currentBit) setLineInput();
                m_byteBitIndex++;
                if (m_byteBitIndex < 8) {
                    writeBit((m_currentByte >> m_byteBitIndex) & 0x01);
                } else {
                    m_state = State::Idle;
                    onWriteByteDone();
                }
            }
            break;

        case State::ReadBit_Start:
            if (e == Event::Timeout) {
                setLineInput();
                m_state = State::ReadBit_ReadSample;
                startTimerUs(10);
            }
            break;

        case State::ReadBit_ReadSample:
            if (e == Event::Timeout) {
                if (readLine())
                    m_currentByte |= (1 << m_byteBitIndex);
                m_byteBitIndex++;
                if (m_byteBitIndex < 8) {
                    m_state = State::Idle;
                    readBit();
                } else {
                    m_state = State::Idle;
                    onReadByteDone();
                }
            }
            break;

        default:
            break;
    }
}

void OW::onTimerElapsed() {
    handleEvent(Event::Timeout);
}

/**
 * Функция-затычка, удалить после тестирования
 */
void OW::resetState() {
    if (m_state == State::Done)
        m_state = State::Idle;
}

void OW::writeBit(bool bit) {
    //if (m_state != State::Idle) return;
    m_currentBit = bit;
    m_state = State::WriteBit_Start;
    setLineOutput();
    writeLine(false);
    startTimerUs(3);
}

void OW::readBit() {
    if (m_state != State::Idle) return;
    m_state = State::ReadBit_Start;
    setLineOutput();
    writeLine(false);
    startTimerUs(3);
}

bool OW::getReadBit() const {
    return m_readBit;
}

void OW::writeByte(uint8_t byte) {
    m_currentByte = byte;
    m_byteBitIndex = 0;
    writeBit(byte & 0x01);
}

void OW::readByteStart() {
    m_currentByte = 0;
    m_byteBitIndex = 0;
    readBit();
}

uint8_t OW::getReadByte() const {
    return m_currentByte;
}

void OW::onWriteByteDone() {

}

void OW::onReadByteDone() {

}
