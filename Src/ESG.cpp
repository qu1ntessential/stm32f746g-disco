/**
 * @file ESG.cpp
 * @brief Файл содержит реализацию класса ESG, инкапсулирующего работу ЭХВЧ аппарата ESG15
 */

#include "ESG.hpp"

extern QueueHandle_t uiEventQueue;
extern QueueHandle_t uiCmdQueue;

/**
 * @brief Алгоритм вычисления контольной суммы CRC8
 * @param buffer Указатель на пакет данных для расчета CRC
 * @param length Количество данных в пакете
 * @return Значение контрольной суммы
 */
uint8_t ESG::crc8(uint8_t *buffer, uint8_t length) {
    uint8_t crc = 0x82;

    while (length--) {
        crc ^= *buffer++;

        for (uint8_t i = 0; i < 8; i++)
            crc = (crc & 1) ? (crc >> 1) ^ 0x8C : crc >> 1;
    }
    return crc;
}

/**
 * @brief Включение платы генератора (подача питания на ПГ)
 */
void ESG::powerOn() {

}

/**
 * @brief Выключение платы генератора (отключение питания ПГ)
 */
void ESG::powerOff() {

}

void ESG::changeMonoBiSel() {
    m_param.isMonoBi = !m_param.isMonoBi;
    //updateLabelText(uiCmdQueue, ID_LABEL_MONO_BI_SEL, m_param.isMonoBi ? "MONO" : "BI");
    print_log(INFO_LOG, "isMonoBi value - %d\r\n", m_param.isMonoBi);
}

uint16_t ESG::checkPowers(uint16_t power, uint8_t mode) {
    if (power > 300 || mode > 2) {
        return 0xFF00;
    }

    power = power & 0x0FFF;
    mode = mode & 0x0F;

    uint16_t data = (static_cast<uint16_t>(mode) << 12) | power;

    return data;
}

bool ESG::setMonoCutPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setMonoCutPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_CUT_POWER, data);
}

bool ESG::setMonoCoagPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setMonoCoagPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_COAG_POWER, data);
}

bool ESG::setBiCutPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setBiCutPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_BCUT_POWER, data);
}

bool ESG::setBiCoagPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setBiCoagPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_BCOAG_POWER, data);
}

bool ESG::setMonoMixPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setMonoMixPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_MIX_POWER, data);
}

bool ESG::setBiMixPower(uint16_t power, uint8_t mode) {
    uint16_t data = checkPowers(power, mode);

    if (data == 0xFF00) {
        print_log(ERROR_LOG, "setBiMixPower received incorrect value or mode\r\n");
        return false;
    }

    return m_twi->putData(I2C::SET_BMIX_POWER, data);
}

bool ESG::setTimeout(uint16_t timeout) {
    return m_twi->putData(I2C::SET_TIMEOUT, timeout);
}

bool ESG::getState() {
    uint8_t buf[10] = {0};
    if (!(m_twi->getData(I2C::GET_STATE, &buf[1], &buf[0]))) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "getData error\r\n");
#endif
        return false;
    }

    m_state.alarm = buf[1];
    m_state.isNE = buf[2] & 1;
    m_state.isOverheat = buf[2] & 2;
    m_state.isPedal = buf[2] & 4;
    m_state.isCrash = buf[2] & 8;
    m_state.isCutActive = buf[2] & 16;
    m_state.isMixActive = buf[2] & 32;
    m_state.isCoagActive = buf[2] & 64;
    m_state.isBiActive = buf[2] & 128;

    return true;
}

void ESG::receiveUiEvents(QueueHandle_t &queue) {
    UIEvent_t event;

    if (xQueueReceive(queue, &event, 0)) {
        switch (event.widget_id) {
            case ID_BTN_MONO_BI_SEL:
                changeMonoBiSel();
                break;
            case ID_BTN_CUT_MIX_PWR_INC:
                break;
            case ID_BTN_CUT_MIX_PWR_DEC:
                break;
            case ID_BTN_CUT_MODE_SEL:
                break;
            case ID_BTN_MIX_MODE_SEL:
                break;
            case ID_BTN_MONOCOAG_PWR_INC:
                break;
            case ID_BTN_MONOCOAG_PWR_DEC:
                break;
            case ID_BTN_MONOCOAG_MODE_SEL:
                break;
            case ID_BTN_BICOAG_PWR_INC:
                break;
            case ID_BTN_BICOAG_PWR_DEC:
                break;
            case ID_BTN_BICOAG_MODE_SEL:
                break;
            case ID_LABEL_CUT_MIX_PWR:
                break;
            case ID_LABEL_MONOCOAG_PWR:
                break;
            case ID_LABEL_BICOAG_PWR:
                break;
            case ID_LABEL_MONO_BI_SEL:

                break;
                /*
            case ID_LABEL_CUT_MIX_PWR_INC:
                break;
            case ID_LABEL_CUT_MIX_PWR_DEC:
                break;
            case ID_LABEL_CUT_MODE_SEL:
                break;
            case ID_LABEL_MIX_MODE_SEL:
                break;
            case ID_LABEL_MONOCOAG_PWR_INC:
                break;
            case ID_LABEL_MONOCOAG_PWR_DEC:
                break;
            case ID_LABEL_MONOCOAG_MODE_SEL:
                break;
            case ID_LABEL_BICOAG_PWR_INC:
                break;
            case ID_LABEL_BICOAG_PWR_DEC:
                break;
            case ID_LABEL_BICOAG_MODE_SEL:
                break;
            default:
                print_log(ERROR_LOG, "Unknown widget ID: %d\r\n", m_event.widget_id);
                break;
                 */
        }
    }
}

void ESG::updateLabelText(QueueHandle_t &queue, uint32_t widget_id, const char *text) {
    UICmd_t cmd;

    cmd.cmd_type = UI_CMD_UPDATE_LABEL;
    cmd.widget_id = widget_id;

    strncpy(cmd.data.text, text, sizeof(cmd.data.text) - 1);
    cmd.data.text[sizeof(cmd.data.text) - 1] = '\0';

    if (xQueueSend(uiCmdQueue, &cmd, pdMS_TO_TICKS(100)) != pdPASS) {
        print_log(ERROR_LOG, "UI cmd queue full!\r\n");
    }
}
