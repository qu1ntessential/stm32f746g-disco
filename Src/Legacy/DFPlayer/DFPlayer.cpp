#include "DFPlayer.hpp"

void DFPlayer::SendCommand(uint8_t cmd, uint8_t param1, uint8_t param2) {
    uint16_t CheckSum = VERSION + CMD_LEN + cmd + FEEDBACK + param1 + param2;
    CheckSum = 0 - CheckSum;

    /**
     * Command Packet: <START> <VER> <Length> <CMD> <Feedback> <param1> <param2> <CheckSum_H> <CheckSum_L> <END>
     */
    uint8_t CommandPacket[10] = {START_BYTE,
                                 VERSION,
                                 CMD_LEN,
                                 cmd,
                                 FEEDBACK,
                                 param1,
                                 param2,
                                 static_cast<uint8_t>((CheckSum >> 8) & 0xFF),
                                 static_cast<uint8_t>((CheckSum & 0xFF)),
                                 END_BYTE};

    HAL_UART_Transmit(m_huart, CommandPacket, sizeof(CommandPacket), HAL_MAX_DELAY);
}

void DFPlayer::Init() {
    SendCommand(INIT_CMD, 0x00, SOURCE_TF_Card);
    //HAL_Delay(1000);
}

void DFPlayer::Play(uint8_t audio_num) {
    SendCommand(0x03, 0x00, audio_num);
}

void DFPlayer::SetVolume(uint8_t volume) {
    SendCommand(0x06, 0x00, volume);
}

void DFPlayer::Next() {
    SendCommand(0x01, 0, 0);
}

void DFPlayer::Previous() {
    SendCommand(0x02, 0, 0);
}

void DFPlayer::Pause() {
    SendCommand(0x0E, 0, 0);
}

void DFPlayer::Resume() {
    SendCommand(0x0D, 0, 0);
}