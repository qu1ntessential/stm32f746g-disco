/**
 * @file FatFsWrapper.hpp
 * @brief Определение класса FatFsWrapper, инкапсулирующего работу с файловой системой FattFS
 */

#ifndef FATFS_WRAPPER_HPP
#define FATFS_WRAPPER_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <memory>
#include <functional>

#include "stm32f7xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ff.h"
#include "sd_diskio_dma.h"
#include "uart_log.h"

/// Макрос, устанавливающий режим полной очистки в функции DeInit()
#define FATFS_FULL_CLEANUP 0

class FatFsWrapper {
public:
    /**
     * @brief Перечисление возможных результатов операций (соответствует кодам FatFs FRESULT)
     */
    enum class Result {
        OK = FR_OK,
        DISK_ERR = FR_DISK_ERR,
        INT_ERR = FR_INT_ERR,
        NOT_READY = FR_NOT_READY,
        NO_FILE = FR_NO_FILE,
        NO_PATH = FR_NO_PATH,
        INVALID_NAME = FR_INVALID_NAME,
        DENIED = FR_DENIED,
        EXIST = FR_EXIST,
        INVALID_OBJECT = FR_INVALID_OBJECT,
        WRITE_PROTECTED = FR_WRITE_PROTECTED,
        INVALID_DRIVE = FR_INVALID_DRIVE,
        NOT_ENABLED = FR_NOT_ENABLED,
        NO_FILESYSTEM = FR_NO_FILESYSTEM,
        MKFS_ABORTED = FR_MKFS_ABORTED,
        TIMEOUT = FR_TIMEOUT,
        LOCKED = FR_LOCKED,
        NOT_ENOUGH_CORE = FR_NOT_ENOUGH_CORE,
        TOO_MANY_OPEN_FILES = FR_TOO_MANY_OPEN_FILES,
        INVALID_PARAMETER = FR_INVALID_PARAMETER
    };

    /// Конструктор класса (по умолчанию)
    FatFsWrapper(Diskio_drvTypeDef *uSD_Driver);
    /// Деструктор класса (обеспечивает корректное освобождение ресурсов)
    ~FatFsWrapper();
    /// Удаляем копирование
    FatFsWrapper(const FatFsWrapper &) = delete;
    /// Удаляем присваивание
    FatFsWrapper &operator=(const FatFsWrapper &) = delete;
    /// Получение внутреннего объекта FATFS (const)
    inline const FATFS &getFsObj() const { return m_fs; }
    /// Получение внутреннего объекта FIL (const)
    inline const FIL &getFileObj() const { return m_file; }

    /// Основные операции
    Result Init();

    Result DeInit();

    /// Файловая система
    Result mount(uint8_t drive = 0);

    Result remount(uint8_t newDrive);

    Result unmount();

    /// Файловые операции
    Result open(const std::string &path, uint8_t mode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS);

    Result close();

    Result read(std::vector<uint8_t> &buffer, uint32_t size);

    Result write(const std::vector<uint8_t> &data);

    Result seek(uint32_t offset);

    Result truncate();

    Result sync();

    /// Операции с директориями
    Result mkdir(const std::string &path);

    Result rmdir(const std::string &path);

    Result ls(const std::string &path, std::vector<std::string> &files);

    /// Файловые утилиты
    Result exists(const std::string &path);

    Result size(const std::string &path, uint32_t &fileSize);

    Result remove(const std::string &path);

    Result rename(const std::string &oldPath, const std::string &newPath);

    /// Утилиты
    static const char *resultToString(Result res);

    Result getLastError() const;

    static std::string getFileExtension(const std::string &path);

    static std::string getFileName(const std::string &path);

    FatFsWrapper::Result testFileOperations(const std::string &path);

private:
    FATFS m_fs;                     ///< Объект файловой системы FatFs
    FIL m_file;                     ///< Объект файла
    DIR m_dir;                      ///< Объект директории
    Result m_lastError;             ///< Последняя ошибка
    bool m_mounted;                 ///< Флаг монтирования ФС
    bool m_fileOpen;                ///< Флаг открытого файла
    bool m_dirOpen;                 ///< Флаг открытой директории
    char m_path[4];                 ///< Текущий путь
    uint8_t m_currentDrive;         ///< Текущий активный диск
    Diskio_drvTypeDef *m_SD_Driver; ///< Указатель на структуру низкоуровневого драйвера диска
    static constexpr uint8_t DEFAULT_DRIVE = 0; ///< Диск по умолчанию
    uint8_t m_workBuffer[_MAX_SS];              ///< Рабочий буфер для операций ФС
    BSP_SD_CardInfo m_CardInfo;                 ///< Информация о SD-карте

    static void LogResult(const char *operation, Result res);
};

#endif // FATFS_WRAPPER_HPP
