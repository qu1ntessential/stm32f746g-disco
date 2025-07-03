#include "DacDriver.hpp"

#define M_PI 3.14

void DacDriver::waveformStop() {
    HAL_TIM_Base_Stop(m_htim);
    HAL_DAC_Stop_DMA(m_hdac, m_channel);
}

void DacDriver::bufferDelete() {
    if (m_waveformBuffer != nullptr) {
        delete[] m_waveformBuffer;
        m_waveformBuffer = nullptr;
        m_bufferSize = 0;
    }
}

HAL_StatusTypeDef DacDriver::waveformGen(const std::vector<uint32_t> &waveform,
                                         float frequency,
                                         uint32_t alignment) {
    // Остановить предыдущую генерацию
    waveformStop();

    // Проверка входных данных
    if (waveform.empty() || frequency <= 0.0f) {
        return HAL_ERROR;
    }

    // Выделить память под буфер
    bufferDelete();
}

HAL_StatusTypeDef DacDriver::sineWaveGen(uint32_t amplitude, uint32_t offset, float frequency, uint32_t points) {
    std::vector<uint32_t> waveform(points);

    for (uint32_t i = 0; i < points; i++) {
        float angle = 2.0f * M_PI * i / points;
        float value = offset + amplitude * sin(angle);
        waveform[i] = static_cast<uint32_t>(value) & 0xFFF;
    }
    return waveformGen(waveform, frequency);
}

HAL_StatusTypeDef DacDriver::triangleWaveGen(uint32_t amplitude, uint32_t offset, float frequency, uint32_t points) {
    return HAL_BUSY;
}

