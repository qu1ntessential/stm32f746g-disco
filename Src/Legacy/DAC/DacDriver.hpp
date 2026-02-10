#pragma once

#include <vector>
#include <cmath>
#include "stm32f7xx.h"

/**
 * @brief Класс для работы с DAC с помощью DMA
 */
class DacDriver {
public:
    enum class WaveformType {
        SINE,
        TRIANGLE,
        SQUARE,
        SAWTOOTH,
        CUSTOM
    };

    /**
     * @brief Конструктор класса DAC_Controller
     * @param hdac Указатель на структуру обработчика DAC
     * @param htim Указатель на обработчик таймера для триггера DAC
     * @param channel Канал DAC (DAC_CHANNEL_1 или DAC_CHANNEL_2)
     */
    explicit DacDriver(DAC_HandleTypeDef *hdac,
                       TIM_HandleTypeDef *htim,
                       uint32_t channel) : m_hdac(hdac),
                                           m_htim(htim),
                                           m_channel(channel),
                                           m_waveformBuffer(nullptr),
                                           m_bufferSize(0),
                                           m_currentFrequency(0.0f),
                                           m_currentWaveform(WaveformType::CUSTOM) {}

    ~DacDriver() {
        waveformStop();
        bufferDelete();
    }

    inline HAL_StatusTypeDef init() { return HAL_DAC_Init(m_hdac); }

    inline HAL_StatusTypeDef start() { return HAL_DAC_Start(m_hdac, m_channel); }

    inline HAL_StatusTypeDef stop() { return HAL_DAC_Stop(m_hdac, m_channel); }

    /**
     * @brief Установка значения DAC (12-битное)
     * @param value Значение от 0 до 4095
     * @param alignment Выравнивание (DAC_ALIGN_12B_R или DAC_ALIGN_12B_L)
     * @retval HAL статус
     */
    inline HAL_StatusTypeDef setValue(uint32_t value, uint32_t alignment = DAC_ALIGN_12B_R) {
        if (value > 0xFFF) value = 0xFFF;
        return HAL_DAC_SetValue(m_hdac, m_channel, alignment, value);
    }

    /**
     * @brief Установка выходного напряжения (в милливольтах)
     * @param voltage_mV Напряжение в милливольтах (0-VREF)
     * @param vref_mV Опорное напряжение в милливольтах (обычно 3300)
     * @retval HAL статус
     */
    inline HAL_StatusTypeDef setVoltage(uint32_t voltage_mV, uint32_t vref_mV = 3300) {
        uint32_t value = (voltage_mV * 4095) / vref_mV;
        return setValue(value);
    }

    /**
     * @brief Остановка генерации сигнала
     */
    void waveformStop();

    /**
     * @brief Генерация периодического сигнала по массиву данных
     * @param waveform Массив данных сигнала (значения от 0 до 4095)
     * @param frequency Частота сигнала в Гц
     * @param alignment Выравнивание данных (DAC_ALIGN_12B_R или DAC_ALIGN_12B_L)
     * @retval HAL-статус
     */
    HAL_StatusTypeDef
    waveformGen(const std::vector<uint32_t> &waveform, float frequency, uint32_t alignment = DAC_ALIGN_12B_R);

    /**
     * @brief Генерация синусоидального сигнала
     * @param amplitude Амплитуда (0 - 4095)
     * @param offset Смещение (0 - 4095)
     * @param frequency Частота в Гц
     * @param points Количество точек на период
     * @retval HAL-статус
     */
    HAL_StatusTypeDef sineWaveGen(uint32_t amplitude, uint32_t offset, float frequency, uint32_t points = 100);

    /**
     * @brief Генерация треугольного сигнала
     * @param amplitude Амплитуда (0 - 4095)
     * @param offset Смещение (0 - 4095)
     * @param frequency Частота в Гц
     * @param points Количество точек на период
     * @retval HAL-статус
     */
    HAL_StatusTypeDef triangleWaveGen(uint32_t amplitude, uint32_t offset, float frequency, uint32_t points = 100);

private:
    DAC_HandleTypeDef *m_hdac;
    TIM_HandleTypeDef *m_htim;
    uint32_t m_channel;
    uint32_t *m_waveformBuffer;
    size_t m_bufferSize;
    float m_currentFrequency;
    WaveformType m_currentWaveform;

    void bufferDelete();

    bool timerConfigForFreq(float frequency, uint32_t points);
};
