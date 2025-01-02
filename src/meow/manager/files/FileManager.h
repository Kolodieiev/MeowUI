#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <Stream.h>

// TODO документація

namespace meow
{
    struct FileInfo
    {
    public:
        FileInfo(const String &name, bool is_dir);
        ~FileInfo() { free(_name); }
        //
        bool isDir() const { return _is_dir; }
        const char *getName() const { return _name; }
        //
        bool operator<(const FileInfo &other) const
        {
            return std::strcmp(_name, other._name) < 0;
        }

        FileInfo(const FileInfo &) = delete;
        FileInfo &operator=(const FileInfo &) = delete;

        FileInfo(FileInfo &&other) noexcept
            : _name(other._name), _is_dir(other._is_dir)
        {
            other._name = nullptr;
        }

        FileInfo &operator=(FileInfo &&other) noexcept
        {
            if (this != &other)
            {
                free(_name);

                _name = other._name;
                _is_dir = other._is_dir;

                other._name = nullptr;
            }
            return *this;
        }

    private:
        char *_name{nullptr};
        bool _is_dir;
    };

    class FileStream : public Stream
    {
    public:
        FileStream(FILE *file, const char *name, size_t size) : _file(file), _name{name}, _size{size} {}
        virtual ~FileStream()
        {
            if (_file)
                fclose(_file);
        }

        virtual int available() override;
        virtual size_t readBytes(char *buffer, size_t length) override;
        virtual int read() override;
        virtual size_t write(uint8_t byte) override;
        virtual int peek() override;
        virtual void flush() override {}

        const char *name() const { return _name.c_str(); }
        size_t size() const { return _size; }

        virtual operator bool() const { return _file != nullptr; }

    private:
        FILE *_file;
        String _name;
        size_t _size;
    };

    typedef std::function<void(bool result, void *arg)> TaskDoneHandler;

    class FileManager
    {
    public:
        /**
         * @brief Перевірити чи примонтовано карту пам'яті. Якщо карту не примонтовано, буде виконана одна спроба монтування.
         *
         * @return true - якщо карта примонтована.
         * @return false - якщо карта не примонтована.
         */
        bool isSdMounted();

        /**
         * @brief Записує в out_path повний шлях до path, з урахуванням точки монтування.
         *
         * @param out_path - Змінна куди буди записано повний шлях.
         * @param path - Шлях, вказаний без точки монтування.
         */
        void makeFullPath(String &out_path, const char *path);

        /**
         * @brief Перевірити чи існує файл на карті пам'яті.
         *
         * @param path Шлях до файлу, вказаний без точки монтування
         * @param silently Прапор, який вказує, чи потрібно виводити повідомлення в консоль, якщо файл відсутній.
         * @return true - якщо файл існує.
         * @return false - якщо файл не існує або є директорією.
         */
        bool fileExist(const char *path, bool silently = false);
        bool dirExist(const char *path, bool silently = false);
        bool exists(const char *path, bool silently = false);
        size_t getFileSize(const char *path);

        bool readStat(struct stat &out_stat, const char *path);

        void indexFilesExt(std::vector<FileInfo> &out_vec, const char *dir_path, const char *file_ext);
        void indexFiles(std::vector<FileInfo> &out_vec, const char *dir_path);
        void indexDirs(std::vector<FileInfo> &out_vec, const char *dir_path);
        void indexAll(std::vector<FileInfo> &out_vec, const char *dir_path);
        //
        bool startRemoving(const char *path);
        bool startCopyFile(const char *from, const char *to);
        //
        void cancel();
        //
        bool rename(const char *old_name, const char *new_name);
        bool createDir(const char *path);
        bool createFile(const char *path);
        //
        bool rmFile(const char *path, bool make_full = false);
        bool rmDir(const char *path);
        //
        size_t readFile(char *out_buffer, const char *path, size_t len, int32_t seek_pos = 0);
        size_t readFromFile(FILE *file, void *out_buffer, size_t len = 1, int32_t seek_pos = 0);
        //
        size_t writeFile(const char *path, const void *buffer, size_t len = 0);
        size_t writeToFile(FILE *file, const void *buffer, size_t len);
        //
        FILE *getFileDescriptor(const char *path, const char *mode);
        void closeFile(FILE *&file);
        bool seekPos(FILE *&file, int32_t pos, uint8_t mode = SEEK_SET);
        size_t getPos(FILE *&file);
        size_t available(size_t size, FILE *file);

        uint8_t getCopyProgress() const { return _copy_progress; }
        //
        void setTaskDoneHandler(TaskDoneHandler handler, void *arg);
        //
        bool isWorking() const { return _is_working; }

    protected:
        enum IndexMode : uint8_t
        {
            INDX_MODE_DIR = 0,
            INDX_MODE_FILES,
            INDX_MODE_FILES_EXT,
            INDX_MODE_ALL
        };

        String _rm_path;
        //
        String _copy_from_path;
        String _copy_to_path;
        uint8_t _copy_progress{0};
        //
    private:
        const uint32_t TASK_SIZE{(1024 / 2) * 40};
        //
        bool _is_working{false};
        bool _is_canceled{false};
        TaskHandle_t _task_handler{nullptr};
        //
        TaskDoneHandler _doneHandler{nullptr};
        void *_doneArg{nullptr};
        //
        uint8_t getEntryType(const char *path, dirent *entry = nullptr);
        //
        void startIndex(std::vector<FileInfo> &out_vec, const char *dir_path, IndexMode mode, const char *file_ext = "");
        //
        void rm();
        void copyFile();
        //
        void taskDone(bool result);
        //
        static void rmTask(void *params);
        static void copyFileTask(void *params);
        //
        size_t writeOptimal(FILE *file, const void *buffer, size_t len);
    };
}