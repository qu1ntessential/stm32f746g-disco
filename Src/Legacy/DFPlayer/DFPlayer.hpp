#ifndef DFPLAYER_HPP
#define DFPLAYER_HPP

#include "stm32f7xx.h"

#define START_BYTE     0x7E
#define VERSION        0xFF
#define CMD_LEN        0x06
#define FEEDBACK       0x00
#define SOURCE_TF_Card 0x02
#define END_BYTE       0xEF
#define INIT_CMD       0x3F

class DFPlayer {
    UART_HandleTypeDef *m_huart{};

    void SendCommand(uint8_t cmd, uint8_t param1, uint8_t param2);

public:
    explicit DFPlayer(UART_HandleTypeDef *huart) : m_huart(huart) {}

    void Init();

    void SetVolume(uint8_t volume);

    void Play(uint8_t audio_num);

    void Next();

    void Pause();

    void Previous();

    void Resume();
};

#endif // DFPLAYER_HPP