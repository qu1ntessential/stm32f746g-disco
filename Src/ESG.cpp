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
    monoCutMode = 1;
    biCutMode = 1;
    monoMixMode = 1;
    biMixMode = 1;
    monoCoagMode = 1;
    biCoagMode = 1;
    monoCutPwr[0] = 0;
    monoCutPwr[1] = 0;
    monoCutPwr[2] = 0;
    monoMixPwr[0] = 0;
    monoMixPwr[1] = 0;
    monoMixPwr[2] = 0;
    monoCoagPwr[0] = 0;
    monoCoagPwr[1] = 0;
    monoCoagPwr[2] = 0;
    biCutPwr[0] = 0;
    biCutPwr[1] = 0;
    biCutPwr[2] = 0;
    biMixPwr[0] = 0;
    biMixPwr[1] = 0;
    biMixPwr[2] = 0;
    biCoagPwr[0] = 0;
    biCoagPwr[1] = 0;
    biCoagPwr[2] = 0;
    timeout = 10;

    //setTimeout();
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
    }
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

uint16_t ESG::getTimeout() const {
    return timeout;
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

void ESG::adjustParam(uint16_t &currentParam, uint16_t minParam, uint16_t maxParam, bool increase, uint16_t step) {
    if (increase) {
        if (currentParam + step <= maxParam) {
            currentParam += step;
        } else {
            currentParam = maxParam;
        }
    } else {
        if (currentParam >= step + minParam) {
            currentParam -= step;
        } else {
            currentParam = minParam;
        }
    }
}

void ESG::changeCutMixPwr(bool increase) {
    if (isCutMix) {
        if (isMonoBi) { /// Текущий режим монополярное резание
            const uint16_t monoCutMin[] = MONOCUT_MIN_PWR;
            const uint16_t monoCutMax[] = MONOCUT_MAX_PWR;
            adjustParam(monoCutPwr[monoCutMode],
                        monoCutMin[monoCutMode],
                        monoCutMax[monoCutMode],
                        increase,
                        PWR_STEP);
            setMonoCutPower();
        } else { /// Текущий режим биполярное резание
            const uint16_t biCutMin[] = BICUT_MIN_PWR;
            const uint16_t biCutMax[] = BICUT_MAX_PWR;
            adjustParam(biCutPwr[biCutMode],
                        biCutMin[biCutMode],
                        biCutMax[biCutMode],
                        increase,
                        PWR_STEP);
            setBiCutPower();
        }
    } else {
        if (isMonoBi) { /// Текущий режим монополярная смесь
            const uint16_t monoMixMin[] = MONOMIX_MIN_PWR;
            const uint16_t monoMixMax[] = MONOMIX_MAX_PWR;
            adjustParam(monoMixPwr[monoMixMode],
                        monoMixMin[monoMixMode],
                        monoMixMax[monoMixMode],
                        increase,
                        PWR_STEP);
            setMonoMixPower();
        } else { /// Текущий режим биполярная смесь
            const uint16_t biMixMin[] = BIMIX_MIN_PWR;
            const uint16_t biMixMax[] = BIMIX_MAX_PWR;
            adjustParam(biMixPwr[biMixMode],
                        biMixMin[biMixMode],
                        biMixMax[biMixMode],
                        increase,
                        PWR_STEP);
            setBiMixPower();
        }
    }
}

void ESG::changeMonoCoagPwr(bool increase) {
    if (isMonoBi && isMonoBiCoag) { /// Текущий режим монополярная коагуляция
        const uint16_t monoCoagMin[] = MONOCOAG_MIN_PWR;
        const uint16_t monoCoagMax[] = MONOCOAG_MAX_PWR;
        adjustParam(monoCoagPwr[monoCoagMode],
                    monoCoagMin[monoCoagMode],
                    monoCoagMax[monoCoagMode],
                    increase,
                    PWR_STEP);
        setMonoCoagPower();
    }
}

void ESG::changeBiCoagPwr(bool increase) {
    if (!isMonoBiCoag) { /// Текущий режим монополярная коагуляция
        const uint16_t biCoagMin[] = BICOAG_MIN_PWR;
        const uint16_t biCoagMax[] = BICOAG_MAX_PWR;
        adjustParam(biCoagPwr[biCoagMode],
                    biCoagMin[biCoagMode],
                    biCoagMax[biCoagMode],
                    increase,
                    PWR_STEP);
        setBiCoagPower();
    }
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

void ESG::changeTimeout(bool increase) {
    adjustParam(timeout,
                MIN_TIMEOUT,
                MAX_TIMEOUT,
                increase,
                TIMEOUT_STEP);
    setTimeout();
}

bool ESG::setTimeout() {
    return m_twi->putData(I2C::SET_TIMEOUT, timeout);
}

/**
 * @brief Функция обновления состояния прибора
 * @return True - получены корректные данные, false - произошла ошибка
 */
bool ESG::getStateTwi() {
    uint8_t buf[5] = {0};
    if (!(m_twi->getData(I2C::GET_STATE, &buf[1], &buf[0]))) {
#if (LL_COM_LOG == 1)
        print_log(ERROR_LOG, "getData error\r\n");
#endif
        return false;
    }

    m_state.alarm = buf[1];
    m_state.isNE = (buf[2] >> 0) & 1;
    m_state.isOverheat = (buf[2] >> 1) & 1;
    m_state.isPedal = (buf[2] >> 2) & 1;
    m_state.isCrash = (buf[2] >> 3) & 1;
    m_state.isCutActive = (buf[2] >> 4) & 1;
    m_state.isMixActive = (buf[2] >> 5) & 1;
    m_state.isCoagActive = (buf[2] >> 6) & 1;
    m_state.isBiActive = (buf[2] >> 7) & 1;

    return true;
}

ESG::States_t ESG::getStateUI() const {
    return m_state;
}

void ESG::syncUI() {
    if (isMonoBi) {
        if (isMonoBiCoag) { /// Активна monoCoag
            lv_obj_remove_state(objects.btn_monocoag_pwr_inc, LV_STATE_DISABLED);
            lv_obj_remove_state(objects.btn_monocoag_pwr_dec, LV_STATE_DISABLED);
            lv_obj_remove_state(objects.btn_monocoag_mode_sel, LV_STATE_DISABLED);

            lv_obj_add_state(objects.btn_bicoag_pwr_inc, LV_STATE_DISABLED);
            lv_obj_add_state(objects.btn_bicoag_pwr_dec, LV_STATE_DISABLED);
        } else { /// Активна biCoag
            lv_obj_remove_state(objects.btn_bicoag_pwr_inc, LV_STATE_DISABLED);
            lv_obj_remove_state(objects.btn_bicoag_pwr_dec, LV_STATE_DISABLED);

            lv_obj_add_state(objects.btn_monocoag_pwr_inc, LV_STATE_DISABLED);
            lv_obj_add_state(objects.btn_monocoag_pwr_dec, LV_STATE_DISABLED);
            lv_obj_remove_state(objects.btn_monocoag_mode_sel, LV_STATE_DISABLED);
        }
    } else { /// Активен режим БПР
        lv_obj_remove_state(objects.btn_bicoag_pwr_inc, LV_STATE_DISABLED);
        lv_obj_remove_state(objects.btn_bicoag_pwr_dec, LV_STATE_DISABLED);

        lv_obj_add_state(objects.btn_monocoag_pwr_inc, LV_STATE_DISABLED);
        lv_obj_add_state(objects.btn_monocoag_pwr_dec, LV_STATE_DISABLED);
        lv_obj_add_state(objects.btn_monocoag_mode_sel, LV_STATE_DISABLED);
    }
}