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

private:
    DAC_HandleTypeDef *m_hdac;
    TIM_HandleTypeDef *m_htim;
    uint32_t m_channel;
    uint32_t *m_waveformBuffer;
    size_t m_bufferSize;
    float m_currentFrequency;
    WaveformType m_currentWaveform;

    HAL_StatusTypeDef
    waveformGen(const std::vector<uint32_t> &waveform, float frequency, uint32_t alignment = DAC_ALIGN_12B_R);

    void bufferDelete();

public:
    explicit DacDriver(DAC_HandleTypeDef *hdac,
                       TIM_HandleTypeDef *htim,
                       uint32_t channel) : m_hdac(hdac),
                                           m_htim(htim),
                                           m_channel(channel),
                                           m_waveformBuffer(nullptr),
                                           m_bufferSize(0),
                                           m_currentFrequency(0.0f),
                                           m_currentWaveform(WaveformType::CUSTOM) {}

    /**
     * @brief Остановка генерации сигнала
     */
    void waveformStop();

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
};
