#ifndef DEVICE_H
#define DEVICE_H

#include <cstring>
#include "I2C.hpp"
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

    typedef struct {
        bool isMonoBi;          ///< Флаг выбора МОНО/БИ режима
        bool isCutMix;          ///< Флаг выбора режима Резание/Смесь
        bool isMonoBiCoag;      ///< Флаг выбора активности в текущий момент МОНО/БИ коагуляции
        uint8_t cutMode;        ///< Режимы резания
        uint8_t mixMode;        ///< Режимы смеси
        uint8_t monoCoagMode;   ///< Режимы МОНО коагуляции
        uint8_t biCoagMode;     ///< Режимы БИ коагуляции
        /// Мощности режимов (18 штук)
        uint16_t monoCut0;
        uint16_t monoCut1;
        uint16_t monoCut2;
        uint16_t biCut0;
        uint16_t biCut1;
        uint16_t biCut2;
        uint16_t monoMix0;
        uint16_t monoMix1;
        uint16_t monoMix2;
        uint16_t biMix0;
        uint16_t biMix1;
        uint16_t biMix2;
        uint16_t monoCoag0;
        uint16_t monoCoag1;
        uint16_t monoCoag2;
        uint16_t biCoag0;
        uint16_t biCoag1;
        uint16_t biCoag2;
    } Params_t;

    States_t m_state; ///< Структура текущего состояния прибора (активность педалей, ошибки и т.д.)
    Params_t m_param; ///< Структура текущих параметров прибора (мощности режимов, активность режимов)

    I2C *m_twi; ///< Указатель на класс, инкапсулирущий работу с I2C

    static uint8_t crc8(uint8_t *buffer, uint8_t length);

    static inline uint16_t checkPowers(uint16_t power, uint8_t mode);

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

    bool setTimeout(uint16_t timeout);

    bool getState();

    void receiveUiEvents(QueueHandle_t &queue);

    void updateLabelText(QueueHandle_t &queue, uint32_t widget_id, const char *text);


};

#endif // DEVICE_H
