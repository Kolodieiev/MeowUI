#pragma GCC optimize("O3")
#include "./FileManager.h"
#include <SD.h>
#include <sys/stat.h>
#include <cstring>
#include <errno.h>

#include "../sd/SD_Manager.h"
#include "FileManager.h"

namespace meow
{
    const char FileManager::STR_DIR_PREFIX[] = "_D ";
    const char STR_TEMP_EXT[] = "_tmp";
    const char STR_DB_EXT[] = ".ldb";
    const char STR_DELIMITER[] = "|";
    const char MOUNT_POINT[] = "/sd";

    bool FileManager::hasConnection()
    {
        bool result = true;

        if (!SD_MNGR.mount() || !SD_MNGR.testConection())
        {
            log_e("Карту пам'яті не примонтовано");
            result = false;
        }

        return result;
    }

    bool FileManager::fileExist(const char *path, bool silently)
    {
        bool result = SD.exists(path);

        if (result)
        {
            File f = SD.open(path);
            if (f.isDirectory())
                result = false;

            f.close();
        }

        if (!result && !silently)
            log_e("File %s not found", path);

        return result;
    }

    bool FileManager::dirExist(const char *path, bool silently)
    {
        bool result = SD.exists(path);

        if (result)
        {
            File f = SD.open(path);
            if (!f.isDirectory())
                result = false;

            f.close();
        }

        if (!result && !silently)
            log_e("Dir %s not found", path);

        return result;
    }

    bool FileManager::exists(const char *path, bool silently)
    {
        bool result = SD.exists(path);
        if (!result && !silently)
            log_e("[ %s ] not exist", path);

        return result;
    }

    bool FileManager::createDir(const char *path)
    {
        bool result = SD.mkdir(path);

        if (!result)
            log_e("Помилка створення директорії: %s", path);

        return result;
    }

    size_t FileManager::readFile(char *out_buffer, const char *path, uint32_t size, uint32_t seek_pos)
    {
        if (!fileExist(path))
            return 0;

        File f = SD.open(path, FILE_READ);
        uint64_t bytes_read = 0;

        if (seek_pos > 0 && !f.seek(seek_pos))
        {
            log_e("Помилка встановлення позиції(%lu) у файлі %s", seek_pos, path);
            goto exit;
        }

        bytes_read = f.read((uint8_t *)out_buffer, size);

    exit:
        f.close();
        return bytes_read;
    }

    bool FileManager::writeFile(const char *path, const char *str_buffer)
    {
        bool result = true;

        File f = SD.open(path, FILE_WRITE, true);

        if (!f)
        {
            log_e("Помилка запису файла %s", path);
            result = false;
            goto exit;
        }

        f.print(str_buffer);

    exit:
        f.close();

        return result;
    }

    size_t FileManager::getFileSize(const char *path)
    {
        if (!fileExist(path))
            return 0;

        File f = SD.open(path, FILE_READ);

        size_t f_size = f.size();
        f.close();

        return f_size;
    }

    void FileManager::rm()
    {
        bool result = false;
        File root = SD.open(_rm_path.c_str());
        bool is_dir = root.isDirectory();
        root.close();

        if (!is_dir)
            result = rmFile(_rm_path.c_str());
        else
        {
            String full_path = MOUNT_POINT;
            full_path += _rm_path;
            result = rmDir(full_path.c_str());
        }

        if (!result)
            log_e("Помилка видалення: %s", _rm_path.c_str());
        else
            _is_successfully = true;

        doFinish();
    }

    bool FileManager::rmFile(const char *path)
    {
        // int core_id = xPortGetCoreID();
        // int core_id = 0;
        // TaskHandle_t idle = xTaskGetIdleTaskHandleForCPU(core_id);

        // if (idle == NULL || esp_task_wdt_delete(idle) != ESP_OK)
        //     log_e("Failed to remove Core %d IDLE task from WDT", core_id);

        bool result = SD.remove(path);

        // if (idle == NULL || esp_task_wdt_add(idle) != ESP_OK)
        //     log_e("Failed to add Core %d IDLE task to WDT", core_id);

        // if (!result)
        //     log_e("Помилка видалення файла: %s", path);

        return result;
    }

    uint8_t FileManager::getEntryType(const char *path, dirent *entry)
    {
        if (entry->d_type != DT_UNKNOWN)
            return entry->d_type;
        else
        {
            // Якщо тип невідомий, уточнюємо через stat
            String full_path = path;
            full_path += "/";
            full_path += entry->d_name;

            struct stat st;
            if (stat(full_path.c_str(), &st) == 0)
            {
                if (S_ISREG(st.st_mode))
                    return DT_REG;
                else if (S_ISDIR(st.st_mode))
                    return DT_DIR;
            }

            return DT_UNKNOWN;
        }
    }

    bool FileManager::rmDir(const char *path)
    {
        errno = 0;
        bool result = false;

        DIR *dir = opendir(path);
        dirent *dir_entry{nullptr};

        if (!dir)
            goto exit;

        while (!_is_canceled)
        {
            dir_entry = readdir(dir);
            if (!dir_entry)
            {
                if (!errno)
                    result = true;
                break;
            }

            if (std::strcmp(dir_entry->d_name, ".") != 0 && std::strcmp(dir_entry->d_name, "..") != 0)
            {
                uint8_t entr_type = getEntryType(path, dir_entry);

                if (entr_type == DT_REG)
                {
                    result = rmFile(dir_entry->d_name);
                    if (!result)
                        goto exit;
                }
                else if (entr_type == DT_DIR)
                {
                    result = rmDir(dir_entry->d_name);

                    if (!result)
                        goto exit;
                }
                else
                {
                    log_e("Невідомий тип або не вдалося прочитати: %s", path);
                    goto exit;
                }
            }

            taskYIELD();
        }

    exit:
        closedir(dir);

        if (result)
        {
            if (!_is_canceled)
                result = SD.rmdir(path);
        }
        else
            log_e("Помилка під час видалення: %s", path);

        return result;
    }

    void FileManager::rmTask(void *params)
    {
        FileManager *instance = static_cast<FileManager *>(params);
        instance->rm();
        vTaskDelete(NULL);
    }

    bool FileManager::startRemoving(const char *path, std::function<void()> on_finish)
    {
        File f = SD.open(path);

        if (!f)
        {
            log_e("Path [ %s ] not found", path);
            return false;
        }

        f.close();

        _rm_path = path;
        _task_finished_callback = on_finish;
        _is_canceled = false;
        _is_successfully = false;

        BaseType_t result = xTaskCreatePinnedToCore(rmTask, "rmTask", TASK_SIZE, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            log_i("rmTask is working now. Path %s", path);
            _is_working = true;
            return true;
        }
        else
        {
            log_e("rmTask was not running");
            return false;
        }
    }

    bool FileManager::rename(const char *old_name, const char *new_name)
    {
        File f = SD.open(old_name);

        if (!f)
        {
            log_e("Path [ %s ] not found", old_name);
            return false;
        }

        f.close();

        return SD.rename(old_name, new_name);
    }

    std::vector<String> FileManager::readFilesFromDB(const char *db_path, uint16_t start_pos, uint16_t size)
    {
        std::vector<String> result;

        if (!fileExist(db_path))
            return result;

        if (size == 0)
            size = 50;

        result.reserve(size);

        File db = SD.open(db_path, FILE_READ);

        db.readStringUntil('|');

        uint16_t pos{0};
        while (pos != start_pos && db.available())
        {
            char ch = db.read();
            if (ch == '|')
                ++pos;
        }

        uint16_t i{0};
        while (db.available() && i < size)
        {
            result.emplace_back(db.readStringUntil('|'));
            ++i;
        }

        db.close();

        return result;
    }

    void FileManager::copyFile()
    {
        bool nf_exist = fileExist(_copy_to_path.c_str(), true);

        if (nf_exist)
        {
            if (!rmFile(_copy_to_path.c_str()))
            {
                doFinish();
                return;
            }
        }

        File new_file = SD.open(_copy_to_path.c_str(), FILE_APPEND, true);

        if (!new_file)
        {
            log_e("Помилка створення файлу [ %s ]", _copy_to_path.c_str());
            doFinish();
            return;
        }

        File old_file = SD.open(_copy_from_path.c_str(), FILE_READ);

        bool has_psram = psramFound() && psramInit();
        size_t buf_size = 1024;
        uint8_t *buffer;

        if (has_psram)
        {
            buf_size *= 500;
            buffer = (uint8_t *)ps_malloc(buf_size);
        }
        else
        {
            buf_size *= 20;
            buffer = (uint8_t *)malloc(buf_size);
        }

        if (!buffer)
        {
            old_file.close();
            new_file.close();
            SD_MNGR.unmount();

            log_e("Помилка виділення пам'яті: %zu b", buf_size);
            esp_restart();
        }

        size_t file_size = old_file.size();
        size_t writed_bytes_counter{0};
        size_t bytes_read;

        uint8_t cycles_counter = 0;

        while (!_is_canceled && old_file.available())
        {
            bytes_read = old_file.read(buffer, buf_size);
            new_file.write(buffer, bytes_read);
            writed_bytes_counter += bytes_read;
            _copy_progress = ((float)writed_bytes_counter / file_size) * 100;

            if (cycles_counter > 10)
            {
                cycles_counter = 0;
                taskYIELD();
            }
            ++cycles_counter;
        }

        free(buffer);

        old_file.close();
        new_file.close();

        if (_is_canceled)
            rmFile(_copy_to_path.c_str());
        else
            _is_successfully = true;

        doFinish();
    }

    void FileManager::copyFileTask(void *params)
    {
        FileManager *instance = static_cast<FileManager *>(params);
        instance->copyFile();
        vTaskDelete(NULL);
    }

    bool FileManager::startCopyingFile(const char *from, const char *to, std::function<void()> on_finish)
    {
        if (!fileExist(from))
            return false;

        _copy_from_path = from;
        _copy_to_path = to;
        _task_finished_callback = on_finish;
        _is_canceled = false;
        _is_successfully = false;
        _copy_progress = 0;

        BaseType_t result = xTaskCreatePinnedToCore(copyFileTask, "copyFileTask", TASK_SIZE, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            log_i("copyFileTask is working now. Path from %s", from);
            _is_working = true;
            return true;
        }
        else
        {
            log_e("copyFileTask was not running");
            return false;
        }
    }

    void FileManager::index()
    {
        // TODO переписати чи хай буде?
        String tmp_db_path = _db_path;
        tmp_db_path += STR_TEMP_EXT;

        if (fileExist(tmp_db_path.c_str(), true))
        {
            if (!rmFile(tmp_db_path.c_str()))
            {
                log_e("Файл заблоковано %s", tmp_db_path);
                doFinish();
                return;
            }
        }

        File tmp_db = SD.open(tmp_db_path, FILE_APPEND, true);

        if (!tmp_db)
        {
            log_e("Помилка відкриття файлу %s", tmp_db_path);
            doFinish();
            return;
        }

        DIR *dir = opendir(_dir_path.c_str());
        dirent *dir_entry{nullptr};

        if (!dir)
        {
            log_e("Помилка відкриття директорії %s", _dir_path.c_str());
            doFinish();
            return;
        }

        String file_name;
        uint16_t counter{0};

        bool is_dir;
        while (!_is_canceled)
        {
            dir_entry = readdir(dir);
            if (!dir_entry)
                break;

            if (std::strcmp(dir_entry->d_name, ".") != 0 && std::strcmp(dir_entry->d_name, "..") != 0)
            {
                uint8_t entr_type = getEntryType(_dir_path.c_str(), dir_entry);

                if (entr_type == DT_REG)
                    is_dir = false;
                else if (entr_type == DT_DIR)
                    is_dir = true;
                else
                    continue;
            }
            else
                continue;

            file_name = dir_entry->d_name;

            switch (_index_mode)
            {
            case INDX_MODE_DIR:
                if (is_dir)
                {
                    file_name += STR_DELIMITER;
                    tmp_db.print(file_name);
                    ++counter;
                }
                break;
            case INDX_MODE_FILES:
                if (!is_dir && !file_name.endsWith(STR_TEMP_EXT) && !file_name.endsWith(STR_DB_EXT))
                {
                    file_name += STR_DELIMITER;
                    tmp_db.print(file_name);
                    ++counter;
                }
                break;
            case INDX_MODE_FILES_EXT:
                if (!is_dir && file_name.endsWith(_file_ext))
                {
                    file_name += STR_DELIMITER;
                    tmp_db.print(file_name);
                    ++counter;
                }
                break;
            case INDX_MODE_ALL:
                if (is_dir)
                    file_name = STR_DIR_PREFIX + file_name;
                else if (file_name.endsWith(STR_TEMP_EXT) || file_name.endsWith(STR_DB_EXT))
                    break;

                file_name += STR_DELIMITER;
                tmp_db.print(file_name);
                ++counter;

                break;
            }

            taskYIELD();
        }

        closedir(dir);
        tmp_db.close();

        if (!_is_canceled)
        {
            if (fileExist(_db_path.c_str(), true))
                rmFile(_db_path.c_str());

            tmp_db = SD.open(tmp_db_path, FILE_READ);
            File db = SD.open(_db_path, FILE_APPEND, true);

            if (!db)
            {
                log_e("Помилка відкриття %s", _db_path);
            }
            else
            {
                String num_str = "";
                num_str += counter;
                num_str += "|";
                db.print(num_str);

                const uint16_t buf_size{1024};
                uint8_t buffer[buf_size];

                size_t bytes_read;
                while (!_is_canceled && tmp_db.available())
                {
                    bytes_read = tmp_db.read(buffer, buf_size);
                    db.write(buffer, bytes_read);
                    taskYIELD();
                }

                db.close();

                if (!_is_canceled)
                    _is_successfully = true;
            }
        }

        tmp_db.close();
        rmFile(tmp_db_path.c_str());

        doFinish();
    }

    void FileManager::indexTask(void *params)
    {
        FileManager *instance = static_cast<FileManager *>(params);
        instance->index();
        vTaskDelete(NULL);
    }

    bool FileManager::startIndexTask(IndexMode mode, const char *dir_path, const char *db_path, std::function<void()> on_finish)
    {
        if (!dirExist(dir_path))
            return false;

        _index_mode = mode;
        _dir_path = MOUNT_POINT;
        _dir_path += dir_path;
        _db_path = db_path;
        _task_finished_callback = on_finish; // TODO Додати аргумент для виклику через this

        _is_canceled = false;
        _is_successfully = false;

        BaseType_t result = xTaskCreatePinnedToCore(indexTask, "indexTask", TASK_SIZE, this, 10, NULL, 1);

        if (result == pdPASS)
        {
            log_i("indexTask is working now. Dir path %s", dir_path);
            _is_working = true;
            return true;
        }
        else
        {
            log_e("indexTask was not running");
            return false;
        }
    }

    bool FileManager::indexFilesExt(const char *dir_path, const char *file_ext, const char *db_path, std::function<void()> on_finish)
    {
        _file_ext = file_ext;
        return startIndexTask(INDX_MODE_FILES_EXT, dir_path, db_path, on_finish);
    }

    bool FileManager::indexFiles(const char *dir_path, const char *db_path, std::function<void()> on_finish)
    {
        return startIndexTask(INDX_MODE_FILES, dir_path, db_path, on_finish);
    }

    bool FileManager::indexDirs(const char *dir_path, const char *db_path, std::function<void()> on_finish)
    {
        return startIndexTask(INDX_MODE_DIR, dir_path, db_path, on_finish);
    }

    bool FileManager::indexAll(const char *dir_path, const char *db_path, std::function<void()> on_finish)
    {
        return startIndexTask(INDX_MODE_ALL, dir_path, db_path, on_finish);
    }

    uint16_t FileManager::getDBSize(const char *db_path)
    {
        if (!fileExist(db_path))
            return 0;

        File f = SD.open(db_path, FILE_READ);

        String temp = f.readStringUntil('|');
        uint16_t result;
        if (temp.isEmpty())
            result = 0;
        else
            result = atoi(temp.c_str());

        f.close();

        return result;
    }

    void FileManager::doFinish()
    {
        _is_working = false;
        if (_task_finished_callback)
            _task_finished_callback();
    }

    void FileManager::cancelTask()
    {
        _is_canceled = true;
    }
}