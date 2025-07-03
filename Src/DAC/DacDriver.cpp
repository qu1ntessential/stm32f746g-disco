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


/**
 * @brief Настройка таймера для нужной частоты обновления DAC
 * @param frequency Частота сигнала в Гц
 * @param points Количество точек на период
 * @return true если настройка прошла успешно
 */
bool DacDriver::timerConfigForFreq(float frequency, uint32_t points) {
    if (frequency <= 0 || points == 0 || m_htim == nullptr) {
        return false;
    }

    // Рассчитать частоту обновления DAC (частота сигнала * количество точек)
    float updateFrequency = frequency * points;

    // Получить частоту таймера
    uint32_t timerClock = HAL_RCC_GetPCLK1Freq();
    if (m_htim->Instance == TIM1 || m_htim->Instance == TIM8 ||
        m_htim->Instance == TIM9 || m_htim->Instance == TIM10 ||
        m_htim->Instance == TIM11) {
        timerClock = HAL_RCC_GetPCLK2Freq();
    }

    // Рассчитать предделитель и период
    uint32_t prescaler = 0;
    uint32_t period = static_cast<uint32_t>(timerClock / updateFrequency) - 1;

    // Если период слишком большой, увеличим предделитель
    if (period > 0xFFFF) {
        prescaler = period / 0xFFFF;
        period = (timerClock / (updateFrequency * (prescaler + 1))) - 1;
    }

    // Настроить таймер
    m_htim->Init.Prescaler = prescaler;
    m_htim->Init.Period = period;
    m_htim->Init.CounterMode = TIM_COUNTERMODE_UP;

    if (HAL_TIM_Base_Init(m_htim) != HAL_OK) {
        return false;
    }

    // Настроить триггер DAC на таймер
    // (Здесь нужно указать правильный триггер для вашего таймера)
    HAL_DACEx_SetConfigSignal(m_hdac, DAC_TRIGGER_T6_TRGO); // Пример для TIM6

    return true;
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
    m_bufferSize = waveform.size();
    m_waveformBuffer = new uint32_t[m_bufferSize];

    // Копировать данные в буфер
    for (size_t i = 0; i < m_bufferSize; i++) {
        m_waveformBuffer[i] = waveform[i] > 4095 ? 4095 : waveform[i];
    }

    // Настроить таймер для нужной частоты
    if (!timerConfigForFreq(frequency, m_bufferSize)) {
        bufferDelete();
        return HAL_ERROR;
    }

    // Сохранить текущие параметры
    m_currentFrequency = frequency;
    m_currentWaveform = WaveformType::CUSTOM;

    // Запустить генерацию сигнала через DMA
    HAL_StatusTypeDef status = HAL_DAC_Start_DMA(m_hdac,
                                                 m_channel,
                                                 m_waveformBuffer,
                                                 m_bufferSize,
                                                 alignment);
    if (status != HAL_OK) {
        bufferDelete();
        return status;
    }

    // Запустить таймер
    return HAL_TIM_Base_Start(m_htim);
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

