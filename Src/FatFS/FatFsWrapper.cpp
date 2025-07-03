/**
 * @file FatFsWrapper.cpp
 * @brief Реализация обертки для работы с файловой системой FatFS
 */

#include "FatFsWrapper.hpp"

/**
 * @brief Конструктор по умолчанию
 * @note Инициализирует все внутренние состояния в значения по умолчанию
 */
FatFsWrapper::FatFsWrapper(const Diskio_drvTypeDef *uSD_Driver) :
        m_lastError(Result::OK),
        m_mounted(false),
        m_fileOpen(false),
        m_dirOpen(false),
        m_currentDrive(DEFAULT_DRIVE),
        m_SD_Driver(uSD_Driver) {}

/**
 * @brief Деструктор
 * @note Автоматически закрывает все открытые ресурсы (файлы, директории, размонтирует ФС)
 */
FatFsWrapper::~FatFsWrapper() {
    /// Закрываем файл, если открыт
    if (m_fileOpen) {
        f_close(&m_file);   /// Явно закрываем файл
        m_fileOpen = false;
    }

    /// Закрываем директорию, если открыта
    if (m_dirOpen) {
        f_closedir(&m_dir);   /// Используем f_closedir из FatFS API
        m_dirOpen = false;
    }

    /// Размонтируем файловую систему, если смонтирована
    if (m_mounted) {
        f_mount(nullptr, m_path, 0);  /// Размонтируем
        m_mounted = false;
    }
}

/**
 * @brief Инициализация файловой системы и SD-карты
 * @return Результат операции (Result::OK при успехе)
 * @note Последовательность операций:
 * 1. Инициализация аппаратного слоя SD-карты
 * 2. Получение информации о карте
 * 3. Регистрация драйвера в FatFS
 * 4. Попытка монтирования существующей ФС
 * 5. При необходимости - создание новой ФС и повторное монтирование
 */
FatFsWrapper::Result FatFsWrapper::Init() {
    snprintf(m_path, sizeof(m_path), "%u:/", m_currentDrive);
    print_log(DEBUG_LOG, "m_path: %s\r\n"
                         "m_currentDrive: %u\r\n", m_path, m_currentDrive);

    /// 1. Инициализация SD/MMC карты
    if (BSP_SD_Init() != MSD_OK) {
        m_lastError = Result::DISK_ERR;
        print_log(ERROR_LOG, "[FatFS] SD card initialization failed (Drive: %u)\r\n", m_currentDrive);
        return m_lastError;
    }

    /// 2. Получение информации о карте
    BSP_SD_GetCardInfo(&m_CardInfo);

    /// Логирование информации о карте
    print_log(INFO_LOG,
              "[FatFS] SD Card Info (Drive: %u):\r\n"
              "  CardType: %lu\r\n"
              "  Class: %lu\r\n"
              "  RelCardAdd: %lu\r\n"
              "  LogBlockSize: %lu\r\n"
              "  LogBlockNbr: %lu bytes\r\n",
              m_currentDrive,
              m_CardInfo.CardType,
              m_CardInfo.Class,
              m_CardInfo.RelCardAdd,
              m_CardInfo.LogBlockSize,
              m_CardInfo.LogBlockNbr
    );

    /// 3. Регистрация драйвера в FatFS
    snprintf(m_path, sizeof(m_path), "%u:/", m_currentDrive);
    if (FATFS_LinkDriver(m_SD_Driver, m_path) != 0) {
        m_lastError = Result::INVALID_DRIVE;
        print_log(ERROR_LOG, "[FatFS] Failed to link driver (Drive: %u)\r\n", m_currentDrive);
        return m_lastError;
    }

    /// 4. Попытка монтирования существующей ФС
    m_lastError = mount(m_currentDrive);
    if (m_lastError == Result::OK) {
        print_log(INFO_LOG, "[FatFS] Filesystem mounted successfully (Drive: %u)\r\n", m_currentDrive);
        return Result::OK;
    }

    /// 5. Создание новой файловой системы при необходимости
    print_log(ERROR_LOG, "[FatFS] Creating new filesystem (Drive: %u)...\r\n", m_currentDrive);

    FRESULT res;

    /// Определение версии FatFS и соответствующего вызова f_mkfs
#if FF_USE_MKFS && FF_FS_EXFAT
    /// Новая версия с поддержкой MKFS_PARM
        MKFS_PARM mkfsParams;
        mkfsParams.fmt = FM_FAT32 | FM_SFD;
        mkfsParams.n_fat = 1;
        mkfsParams.align = 0;
        mkfsParams.n_root = 512;
        mkfsParams.au_size = 0;

        res = f_mkfs(m_path, &mkfsParams, m_workBuffer, sizeof(m_workBuffer));
#else
    /// Старая версия FatFS (без MKFS_PARM)
    res = f_mkfs(m_path, FM_FAT32, 0, m_workBuffer, sizeof(m_workBuffer));
#endif

    if (res != FR_OK) {
        m_lastError = static_cast<Result>(res);
        print_log(ERROR_LOG, "[FatFS] f_mkfs failed: %s (Drive: %u)\r\n",
                  resultToString(m_lastError), m_currentDrive);
        FATFS_UnLinkDriver(m_path);
        return m_lastError;
    }

    /// 6. Повторное монтирование после создания ФС
    m_lastError = mount(m_currentDrive);
    if (m_lastError != Result::OK) {
        print_log(ERROR_LOG, "[FatFS] Failed to mount after mkfs: %s (Drive: %u)\r\n",
                  resultToString(m_lastError), m_currentDrive);
        FATFS_UnLinkDriver(m_path);
        return m_lastError;
    }

    print_log(INFO_LOG, "[FatFS] New filesystem created and mounted (Drive: %u)\r\n", m_currentDrive);
    return Result::OK;
}

/**
 * @brief Деинициализация файловой системы и освобождение ресурсов
 * @return Результат операции (Result::OK при успехе)
 * @note Последовательность операций:
 * 1. Размонтирование файловой системы
 * 2. Очистка карты (опционально)
 * 3. Отключение драйвера
 * 4. Деинициализация SD-карты
 * 5. Сброс всех состояний
 */
FatFsWrapper::Result FatFsWrapper::DeInit() {
    Result result = Result::OK;
    uint8_t current_drive = m_currentDrive; // Сохраняем текущий диск

    /// 1. Размонтирование файловой системы
    if (m_mounted) {
        result = unmount();
        if (result != Result::OK) {
            print_log(ERROR_LOG, "[FatFS] Unmount failed: %s (Drive: %u)\r\n",
                      resultToString(result), current_drive);
        }
    }

    /// 2. Очистка карты (только в режиме полной очистки)
#if (FATFS_FULL_CLEANUP == 1)
    if (BSP_MMC_IsDetected(current_drive)) {
        uint32_t erase_result = BSP_MMC_Erase(current_drive, 0, m_discoCardInfo.LogBlockNbr - 1);
        if (erase_result != BSP_ERROR_NONE) {
            print_log(WARN_LOG, "[FatFS] Partial erase failed: %lu (Drive: %u)\r\n",
                   erase_result, current_drive);
            if (result == Result::OK) {
                result = Result::DISK_ERR;
            }
        }
    }
#endif

    /// 3. Отключение драйвера FatFS
    if (FATFS_UnLinkDriver(m_path) != 0) {
        print_log(ERROR_LOG, "[FatFS] Failed to unlink driver (Drive: %u)\r\n", current_drive);
        if (result == Result::OK) {
            result = Result::INT_ERR;
        }
    }

    /// 4. Деинициализация SD-карты
    if (BSP_SD_DeInit() != MSD_OK) {
        print_log(ERROR_LOG, "[FatFS] SD card deinit failed (Drive: %u)\r\n", current_drive);
        if (result == Result::OK) {
            result = Result::DISK_ERR;
        }
    }

    /// 5. Сброс всех состояний
    m_mounted = false;
    m_fileOpen = false;
    m_dirOpen = false;
    m_lastError = Result::OK;
    // Не сбрасываем m_currentDrive для сохранения контекста

    print_log(INFO_LOG, "[FatFS] Deinitialized successfully (Drive: %u)\r\n", current_drive);
    return result;
}

/**
 * @brief Монтирование файловой системы
 * @param drive Номер диска (по умолчанию 0)
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::mount(uint8_t drive) {
    /// Проверка на уже смонтированный диск
    if (m_mounted) {
        if (drive == m_currentDrive) {
            return Result::OK;   /// Уже смонтирован нужный диск
        }
        /// Размонтировать текущий диск перед сменой
        Result res = unmount();
        if (res != Result::OK) {
            return res;
        }
    }

    /// Обновление текущего диска
    m_currentDrive = drive;
    snprintf(m_path, sizeof(m_path), "%u:/", m_currentDrive);

    FRESULT res = f_mount(&m_fs, m_path, 1);
    m_lastError = static_cast<Result>(res);

    if (res == FR_OK) {
        m_mounted = true;
        print_log(INFO_LOG, "Mounted drive %u\r\n", m_currentDrive);
    }

    return m_lastError;
}

/**
 * @brief
 * @param newDrive
 * @return
 */
FatFsWrapper::Result FatFsWrapper::remount(uint8_t newDrive) {
    Result res = unmount();
    if (res != Result::OK) return res;
    return mount(newDrive);
}

/**
 * @brief Размонтирование файловой системы
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::unmount() {
    if (!m_mounted) return Result::OK;

    FRESULT res = f_mount(nullptr, "", 0);
    m_lastError = static_cast<Result>(res);

    if (res == FR_OK) {
        m_mounted = false;
    }

    return m_lastError;
}

/**
 * @brief Открытие файла
 * @param path Путь к файлу
 * @param mode Режим открытия (по-умолчанию FA_READ | FA_WRITE | FA_OPEN_ALWAYS)
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::open(const std::string &path, uint8_t mode) {
    if (m_fileOpen) close();

    FRESULT res = f_open(&m_file, path.c_str(), FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
    m_lastError = static_cast<Result>(res);

    if (res == FR_OK) {
        m_fileOpen = true;
    }

    return m_lastError;
}

/**
 * @brief Закрытие файла
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::close() {
    if (!m_fileOpen) return Result::OK;

    FRESULT res = f_close(&m_file);
    m_lastError = static_cast<Result>(res);

    if (res == FR_OK) {
        m_fileOpen = false;
    }

    return m_lastError;
}

/**
 * @brief Чтение из файла
 * @param[out] buffer Буфер для данных
 * @param size Количество байт для чтения
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::read(std::vector<uint8_t> &buffer, uint32_t size) {
    if (!m_fileOpen) return Result::NOT_READY;

    buffer.resize(size);
    UINT bytesRead = 0;

    FRESULT res = f_read(&m_file, buffer.data(), size, &bytesRead);
    m_lastError = static_cast<Result>(res);

    if (res == FR_OK) {
        buffer.resize(bytesRead);
    } else {
        buffer.clear();
    }

    return m_lastError;
}

/**
 * @brief Запись в файл
 * @param data Данные для записи
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::write(const std::vector<uint8_t> &data) {
    if (!m_fileOpen) return Result::NOT_READY;

    UINT bytesWritten = 0;
    FRESULT res = f_write(&m_file, data.data(), data.size(), &bytesWritten);
    m_lastError = static_cast<Result>(res);

    return m_lastError;
}

/**
 * @brief Получение строкового представления кода ошибки
 * @param res Код ошибки
 * @return Строковое описание ошибки
 */
const char *FatFsWrapper::resultToString(FatFsWrapper::Result res) {
    static const char *m_fResults[] = {
            "FR_OK",
            "FR_DISK_ERR",
            "FR_INT_ERR",
            "FR_NOT_READY",
            "FR_NO_FILE",
            "FR_NO_PATH",
            "FR_INVALID_NAME",
            "FR_DENIED",
            "FR_EXIST",
            "FR_INVALID_OBJECT",
            "FR_WRITE_PROTECTED",
            "FR_INVALID_DRIVE",
            "FR_NOT_ENABLED",
            "FR_NO_FILESYSTEM",
            "FR_MKFS_ABORTED",
            "FR_TIMEOUT",
            "FR_LOCKED",
            "FR_NOT_ENOUGH_CORE",
            "FR_TOO_MANY_OPEN_FILES",
            "FR_INVALID_PARAMETER"
    };

    return m_fResults[static_cast<int>(res)];
}

/**
 * @brief Возвращает код последней ошибки
 * @return Код последней ошибки (enum class Result)
 */
FatFsWrapper::Result FatFsWrapper::getLastError() const {
    return m_lastError;
}

/**
 * @brief Перемещение позиции в файле
 * @param offset Смещение в байтах
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::seek(uint32_t offset) {
    if (!m_fileOpen) return Result::NOT_READY;

    FRESULT res = f_lseek(&m_file, offset);
    m_lastError = static_cast<Result>(res);
    LogResult("f_lseek", m_lastError);
    return m_lastError;
}

/**
 * @brief Усечение файла до текущей позиции
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::truncate() {
    if (!m_fileOpen) return Result::NOT_READY;

    FRESULT res = f_truncate(&m_file);
    m_lastError = static_cast<Result>(res);
    LogResult("f_truncate", m_lastError);
    return m_lastError;
}

/**
 * @brief Синхронизация изменений файла
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::sync() {
    if (!m_fileOpen) return Result::NOT_READY;

    FRESULT res = f_sync(&m_file);
    m_lastError = static_cast<Result>(res);
    LogResult("f_sync", m_lastError);
    return m_lastError;
}

/**
 * @brief Создание директории
 * @param path Путь к директории
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::mkdir(const std::string &path) {
    FRESULT res = f_mkdir(path.c_str());
    m_lastError = static_cast<Result>(res);
    LogResult("f_mkdir", m_lastError);
    return m_lastError;
}

/**
 * @brief Удаление директории
 * @param path Путь к директории
 * @return Результат операции
 * @note Директория должна быть пустой
 */
FatFsWrapper::Result FatFsWrapper::rmdir(const std::string &path) {
    FRESULT res = f_unlink(path.c_str());
    m_lastError = static_cast<Result>(res);
    LogResult("f_unlink (rmdir)", m_lastError);
    return m_lastError;
}

/**
 * @brief Получение списка файлов в директории
 * @param path Путь к директории
 * @param[out] files Вектор для хранения имен файлов
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::ls(const std::string &path, std::vector<std::string> &files) {
    files.clear();
    FILINFO fno;
    DIR dir;

    FRESULT res = f_opendir(&dir, path.c_str());
    if (res != FR_OK) {
        m_lastError = static_cast<Result>(res);
        LogResult("f_opendir", m_lastError);
        return m_lastError;
    }

    while (true) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;

        if (!(fno.fattrib & AM_HID)) {
            files.emplace_back(fno.fname);
        }
    }

    f_closedir(&dir);
    m_lastError = static_cast<Result>(res);
    LogResult("f_readdir", m_lastError);
    return m_lastError;
}

/**
 * @brief Проверка существования файла/директории
 * @param path Путь к проверяемому объекту
 * @return Результат операции (Result::OK если существует)
 */
FatFsWrapper::Result FatFsWrapper::exists(const std::string &path) {
    FILINFO fno;
    FRESULT res = f_stat(path.c_str(), &fno);
    m_lastError = static_cast<Result>(res);
    LogResult("f_stat (exists)", m_lastError);
    return m_lastError;
}

/**
 * @brief Получение размера файла
 * @param path Путь к файлу
 * @param[out] fileSize Переменная для хранения размера
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::size(const std::string &path, uint32_t &fileSize) {
    FILINFO fno;
    FRESULT res = f_stat(path.c_str(), &fno);
    if (res == FR_OK) {
        fileSize = fno.fsize;
    }
    m_lastError = static_cast<Result>(res);
    LogResult("f_stat (size)", m_lastError);
    return m_lastError;
}

/**
 * @brief Удаление файла
 * @param path Путь к файлу
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::remove(const std::string &path) {
    FRESULT res = f_unlink(path.c_str());
    m_lastError = static_cast<Result>(res);
    LogResult("f_unlink (remove)", m_lastError);
    return m_lastError;
}

/**
 * @brief Переименование файла/директории
 * @param oldPath Старый путь
 * @param newPath Новый путь
 * @return Результат операции
 */
FatFsWrapper::Result FatFsWrapper::rename(const std::string &oldPath, const std::string &newPath) {
    FRESULT res = f_rename(oldPath.c_str(), newPath.c_str());
    m_lastError = static_cast<Result>(res);
    LogResult("f_rename", m_lastError);
    return m_lastError;
}

/**
 * @brief Получение расширения файла
 * @param path Путь к файлу
 * @return Строка с расширением (без точки)
 */
std::string FatFsWrapper::getFileExtension(const std::string &path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos + 1);
    }
    return "";
}

/**
 * @brief Получение имени файла из пути
 * @param path Полный путь
 * @return Имя файла с расширением
 */
std::string FatFsWrapper::getFileName(const std::string &path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return path.substr(slashPos + 1);
    }
    return path;
}

/**
 * @brief Логирование результата операции
 * @param operation Название операции
 * @param res Результат операции
 * @note Выводит в лог сообщение об успехе/ошибке
 */
void FatFsWrapper::LogResult(const char *operation, Result res) {
    if (res != Result::OK) {
        print_log(ERROR_LOG, "%s failed: %s\r\n", operation, resultToString(res));
    } else {
        print_log(DEBUG_LOG, "%s succeeded\r\n", operation);
    }
}