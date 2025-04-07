#ifndef ESG_DEVICE_HPP
#define ESG_DEVICE_HPP

#include <cstring>
#include "I2C.hpp"
#include "config.h"
#include "ui_con.h"

#define LL_COM_LOG 0

class ESG {
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

    States_t m_state; ///< Структура текущего состояния прибора (активность педалей, ошибки и т.д.)

    I2C *m_twi; ///< Указатель на класс, инкапсулирущий работу с I2C

    static uint8_t crc8(uint8_t *buffer, uint8_t length);

    static inline bool convertData(uint16_t power, uint8_t mode, I2C::Orders order, uint16_t *data);

public:
    explicit ESG(I2C *twi) : m_twi(twi) {}

    void powerOn();

    void powerOff();

    void changeMonoBiSel();

    bool setMonoCutPower(uint16_t power, uint8_t mode);

    bool setMonoCoagPower(uint16_t power, uint8_t mode);

    bool setMonoMixPower(uint16_t power, uint8_t mode);

    bool setBiCutPower(uint16_t power, uint8_t mode);

    bool setBiCoagPower(uint16_t power, uint8_t mode);

    bool setBiMixPower(uint16_t power, uint8_t mode);

    void getMonoCutPower(char *buf, uint8_t len);

    bool getMonoCoagPower();

    bool getMonoMixPower();

    bool getBiCutPower();

    bool getBiCoagPower();

    bool getBiMixPower();

    bool setTimeout(uint16_t timeout);

    bool getState();

    void receiveUiEvents(QueueHandle_t &queue);

    void updateLabelText(QueueHandle_t &queue, uint32_t widget_id, const char *text);


};

#endif // ESG_DEVICE_HPP
