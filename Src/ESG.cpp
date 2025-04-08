/**
 * @file ESG.cpp
 * @brief Файл содержит реализацию класса ESG, инкапсулирующего работу ЭХВЧ аппарата ESG15
 */

#include "ESG.hpp"

void ESG::Init() {
    /** Здесь должна быть реализация, инициализирующая нижеследующие поля значениями из памяти
        (QSPI или EEPROM)
     */
    isMonoBi = true;
    isCutMix = false;
    isMonoBiCoag = true;
    monoCutMode = 0;
    biCutMode = 1;
    monoMixMode = 2;
    biMixMode = 0;
    monoCoagMode = 1;
    biCoagMode = 2;
    monoCutPwr[0] = 1;
    monoCutPwr[1] = 2;
    monoCutPwr[2] = 3;
    monoMixPwr[0] = 4;
    monoMixPwr[1] = 5;
    monoMixPwr[2] = 300;
    monoCoagPwr[0] = 7;
    monoCoagPwr[1] = 8;
    monoCoagPwr[2] = 9;
    biCutPwr[0] = 10;
    biCutPwr[1] = 11;
    biCutPwr[2] = 12;
    biMixPwr[0] = 13;
    biMixPwr[1] = 14;
    biMixPwr[2] = 15;
    biCoagPwr[0] = 16;
    biCoagPwr[1] = 17;
    biCoagPwr[2] = 18;
    timeout = 10;
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

void ESG::invMonoBiSel() {
    isMonoBi = !isMonoBi;
    if (!isMonoBi) { /// Выбран биполярный режим
        isMonoBiCoag = false;
    } else {
        isMonoBiCoag = true;
    }
}

bool ESG::convertData(uint16_t power, uint8_t mode, I2C::Orders order, uint16_t *data) {
    /// Проверка на nullptr
    if (!data) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Received nullptr\r\n");
#endif
        return false;
    }

    /// Проверка допустимости режима
    if (mode > 2) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Incorrect mode (must be 0, 1, or 2)\r\n");
#endif
        return false;
    }

    /// Lookup-таблицы для минимальных и максимальных значений мощности
    static constexpr uint16_t MIN_PWR[6][3] = {
            {MONOCUT0_MIN_PWR,  MONOCUT1_MIN_PWR,  MONOCUT2_MIN_PWR},
            {MONOMIX0_MIN_PWR,  MONOMIX1_MIN_PWR,  MONOMIX2_MIN_PWR},
            {MONOCOAG0_MIN_PWR, MONOCOAG1_MIN_PWR, MONOCOAG2_MIN_PWR},
            {BICUT0_MIN_PWR,    BICUT1_MIN_PWR,    BICUT2_MIN_PWR},
            {BIMIX0_MIN_PWR,    BIMIX1_MIN_PWR,    BIMIX2_MIN_PWR},
            {BICOAG0_MIN_PWR,   BICOAG1_MIN_PWR,   BICOAG2_MIN_PWR}
    };

    static constexpr uint16_t MAX_PWR[6][3] = {
            {MONOCUT0_MAX_PWR,  MONOCUT1_MAX_PWR,  MONOCUT2_MAX_PWR},
            {MONOMIX0_MAX_PWR,  MONOMIX1_MAX_PWR,  MONOMIX2_MAX_PWR},
            {MONOCOAG0_MAX_PWR, MONOCOAG1_MAX_PWR, MONOCOAG2_MAX_PWR},
            {BICUT0_MAX_PWR,    BICUT1_MAX_PWR,    BICUT2_MAX_PWR},
            {BIMIX0_MAX_PWR,    BIMIX1_MAX_PWR,    BIMIX2_MAX_PWR},
            {BICOAG0_MAX_PWR,   BICOAG1_MAX_PWR,   BICOAG2_MAX_PWR}
    };

    /// Проверка мощности
    if (order < I2C::SET_MONO_CUT_PWR || order > I2C::SET_BI_COAG_PWR) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Invalid order\r\n");
#endif
        return false;
    }

    uint8_t order_index = order - I2C::SET_MONO_CUT_PWR;
    bool powerValid = (power >= MIN_PWR[order_index][mode]) &&
                      (power <= MAX_PWR[order_index][mode]);

    if (!powerValid) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "Power %d out of range for mode %d and order %d\r\n",
                  power, mode, order);
#endif
        return false;
    }

    /// Маскирование и упаковка данных
    power = power & 0x0FFF;
    mode = mode & 0x0F;
    *data = (static_cast<uint16_t>(mode) << 12) | power;
    return true;
}

uint16_t ESG::createData(uint16_t power, uint8_t mode) {
    /// Маскирование и упаковка данных
    power = power & 0x0FFF;
    mode = mode & 0x0F;
    return ((static_cast<uint16_t>(mode) << 12) | power);
}

bool ESG::setMonoCutPower() const {
    uint16_t data = createData(monoCutPwr[monoCutMode], monoCutMode);
    return m_twi->putData(I2C::SET_MONO_CUT_PWR, data);
}

bool ESG::setMonoCoagPower() const {
    uint16_t data = createData(monoCoagPwr[monoCoagMode], monoCoagMode);
    return m_twi->putData(I2C::SET_MONO_COAG_PWR, data);
}

bool ESG::setBiCutPower() const {
    uint16_t data = createData(biCutPwr[biCutMode], biCutMode);
    return m_twi->putData(I2C::SET_BI_CUT_PWR, data);
}

bool ESG::setBiCoagPower() const {
    uint16_t data = createData(biCoagPwr[biCoagMode], biCoagMode);
    return m_twi->putData(I2C::SET_BI_COAG_PWR, data);
}

bool ESG::setMonoMixPower() const {
    uint16_t data = createData(monoMixPwr[monoMixMode], monoMixMode);
    return m_twi->putData(I2C::SET_MONO_MIX_PWR, data);
}

bool ESG::setBiMixPower() const {
    uint16_t data = createData(biMixPwr[biMixMode], biMixMode);
    return m_twi->putData(I2C::SET_BI_MIX_PWR, data);
}

uint16_t ESG::getCutMixPower() const {
    if (isMonoBi) {
        if (isCutMix) {
            return monoCutPwr[monoCutMode];
        } else {
            return monoMixPwr[monoMixMode];
        }
    } else {
        if (isCutMix) {
            return biCutPwr[biCutMode];
        } else {
            return biMixPwr[biMixMode];
        }
    }
}

uint16_t ESG::getMonoCoagPower() const {
    return monoCoagPwr[monoCoagMode];
}

uint16_t ESG::getBiCoagPower() const {
    return biCoagPwr[biCoagMode];
}

uint8_t ESG::getCutMode() const {
    if (isMonoBi) {
        return monoCutMode;
    } else {
        return biCutMode;
    }
}

uint8_t ESG::getMixMode() const {
    if (isMonoBi) {
        return monoMixMode;
    } else {
        return biMixMode;
    }
}

uint8_t ESG::getMonoCoagMode() const {
    return monoCoagMode;
}

uint8_t ESG::getBiCoagMode() const {
    return biCoagMode;
}

[[nodiscard]] bool ESG::getMonoBiFlag() const {
    return isMonoBi;
}

void ESG::changeCutMixPwr(bool incDec) {
    if ()
}

void ESG::changeMonoCoagPwr(bool incDec) {

}

void ESG::changeBiCoagPwr(bool incDec) {

}

void ESG::changeCutMode() {
    if (isCutMix) {
        if (isMonoBi) { /// Текущий режим монополярное резание
            if (++monoCutMode > 2)
                monoCutMode = 0;
        } else { /// Текущий режим биполярное резание
            if (++biCutMode > 2)
                biCutMode = 0;
        }
    } else {
        isCutMix = true; /// Переключаем текущий режим на резание
    }
}

void ESG::changeMixMode() {
    if (!isCutMix) {
        if (isMonoBi) { /// Текущий режим монополярная смесь
            if (++monoMixMode > 2)
                monoMixMode = 0;
        } else { /// Текущий режим биполярная смесь
            if (++biMixMode > 2)
                biMixMode = 0;
        }
    } else {
        isCutMix = false; /// Переключаем текущий режим на смесь
    }
}

void ESG::changeMonoCoagMode() {
    if (isMonoBi) {
        if (isMonoBiCoag) { /// Текущий режим монополярная коагуляция
            if (++monoCoagMode > 2)
                monoCoagMode = 0;
        } else { /// Текущий режим биполярная коагуляция
            isMonoBiCoag = true; /// Переключаем текущий режим на монополярную коагуляцию
        }
    }
    /**
     * Нет ветви else, т.к. в режиме БПР не предусмотрено
     * использование монополярной коагуляции
     */
}

void ESG::changeBiCoagMode() {
    if (isMonoBi) {
        if (isMonoBiCoag) { /// Текущий режим монополярная коагуляция
            isMonoBiCoag = false; /// Переключаем текущий режим на биполярную коагуляцию
        } else { /// Текущий режим биполярная коагуляция
            if (++biCoagMode > 2)
                biCoagMode = 0;
        }
    } else {
        isMonoBiCoag = false;
        if (++biCoagMode > 2)
            biCoagMode = 0;
    }
}

bool ESG::setTimeout(uint16_t timeOut) {
    if (timeOut > MAX_TIMEOUT) return false;
    timeout = timeOut;
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
