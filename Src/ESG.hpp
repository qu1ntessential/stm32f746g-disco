#ifndef ESG_DEVICE_HPP
#define ESG_DEVICE_HPP

#include <cstring>
#include "I2C.hpp"
#include "config.h"
#include "ui_con.h"

#include "stm32746g_discovery_qspi.h"

#define LL_COM_LOG 1

class ESG {
public:
    typedef struct {
        uint16_t alarm: 8;           ///< Alarm code
        uint16_t isNE: 1;            ///< Neutral electrode is connected
        uint16_t isOverheat: 1;      ///< Overheating of power stage
        uint16_t isPedal: 1;         ///< Single-key pedal (pedal 2) is connected
        uint16_t isCrash: 1;         ///< Program is reset
        uint16_t isCutActive: 1;
        uint16_t isMixActive: 1;
        uint16_t isCoagActive: 1;
        uint16_t isBiActive: 1;
    } States_t;

    explicit ESG(I2C *twi) : m_twi(twi) {}

    void Init();

    void setDefaultParams();

    void powerOn();

    void powerOff();

    void invMonoBiSel();

    /**
     * @defgroup Функции установки мощности и активного режима согласно текущим
     *           переменным класса (подразумевается, что данные корректные)
     */
    [[nodiscard]] bool setMonoCutPower() const;

    [[nodiscard]] bool setMonoCoagPower() const;

    [[nodiscard]] bool setMonoMixPower() const;

    [[nodiscard]] bool setBiCutPower() const;

    [[nodiscard]] bool setBiCoagPower() const;

    [[nodiscard]] bool setBiMixPower() const;

    /**
     * @defgroup Методы для передачи мощности и активного режима в UI
     */
    [[nodiscard]] uint16_t getCutMixPower() const;

    [[nodiscard]] uint16_t getMonoCoagPower() const;

    [[nodiscard]] uint16_t getBiCoagPower() const;

    [[nodiscard]] uint8_t getCutMode() const;

    [[nodiscard]] uint8_t getMixMode() const;

    [[nodiscard]] uint8_t getMonoCoagMode() const;

    [[nodiscard]] uint8_t getBiCoagMode() const;

    [[nodiscard]] bool getMonoBiFlag() const;

    /**
     * @defgroup Методы для передачи зменения мощности и режима в UI
     */
    void changeCutMixPwr(bool increase);

    void changeMonoCoagPwr(bool increase);

    void changeBiCoagPwr(bool increase);

    void changeCutMode();

    void changeMixMode();

    void changeMonoCoagMode();

    void changeBiCoagMode();

    bool setTimeout(uint16_t timeOut);

    bool getStateTwi();

    [[nodiscard]] ESG::States_t getStateUI() const;

    void checkStateUI() const;

private:
    bool isMonoBi;          ///< Флаг выбора МОНО/БИ режима
    bool isCutMix;          ///< Флаг выбора режима Резание/Смесь
    bool isMonoBiCoag;      ///< Флаг выбора активности в текущий момент МОНО/БИ коагуляции
    uint8_t monoCutMode;    ///< Режимы МОНО резания
    uint8_t biCutMode;      ///< Режимы БИ резания
    uint8_t monoMixMode;    ///< Режимы МОНО смеси
    uint8_t biMixMode;      ///< Режимы БИ смеси
    uint8_t monoCoagMode;   ///< Режимы МОНО коагуляции
    uint8_t biCoagMode;     ///< Режимы БИ коагуляции
    /// Мощности режимов (18 штук)
    uint16_t monoCutPwr[3];
    uint16_t biCutPwr[3];
    uint16_t monoMixPwr[3];
    uint16_t biMixPwr[3];
    uint16_t monoCoagPwr[3];
    uint16_t biCoagPwr[3];
    uint8_t timeout;

    States_t m_state; ///< Структура текущего состояния прибора (активность педалей, ошибки и т.д.)

    I2C *m_twi; ///< Указатель на класс, инкапсулирущий работу с I2C

    static inline uint16_t createData(uint16_t power, uint8_t mode);

    static void adjustPower(uint16_t &currentPower, uint16_t minPower, uint16_t maxPower, bool increase);
};

#endif // ESG_DEVICE_HPP
