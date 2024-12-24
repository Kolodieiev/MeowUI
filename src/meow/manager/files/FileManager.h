#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <vector>

namespace meow
{
    class FileManager
    {
    public:
        static const char STR_DIR_PREFIX[];

        bool hasConnection();

        bool fileExist(const char *path, bool silently = false);
        bool dirExist(const char *path, bool silently = false);
        bool exists(const char *path, bool silently = false);

        bool indexFilesExt(const char *dir_path, const char *file_ext, const char *db_path, std::function<void()> on_finish = nullptr);
        bool indexFiles(const char *dir_path, const char *db_path, std::function<void()> on_finish = nullptr);
        bool indexDirs(const char *dir_path, const char *db_path, std::function<void()> on_finish = nullptr);
        bool indexAll(const char *dir_path, const char *db_path, std::function<void()> on_finish = nullptr);

        bool startRemoving(const char *path, std::function<void()> on_finish = nullptr);
        bool startCopyingFile(const char *from, const char *to, std::function<void()> on_finish = nullptr);

        void cancelTask();

        bool rename(const char *old_name, const char *new_name);

        bool createDir(const char *path);

        size_t readFile(char *out_buffer, const char *path, uint32_t size, uint32_t seek_pos = 0);

        bool writeFile(const char *path, const char *str_buffer);

        size_t getFileSize(const char *path);

        uint16_t getDBSize(const char *db_path);
        std::vector<String> readFilesFromDB(const char *db_path, uint16_t start_pos, uint16_t size);

        bool isWorking() const { return _is_working; }
        bool isTaskSuccessfull() const { return _is_successfully; }
        uint8_t getCopyProgress() const { return _copy_progress; }

    protected:
        enum IndexMode : uint8_t
        {
            INDX_MODE_DIR = 0,
            INDX_MODE_FILES,
            INDX_MODE_FILES_EXT,
            INDX_MODE_ALL
        };

        bool _is_working = false;
        bool _is_canceled = false;
        bool _is_successfully = false;

        String _rm_path;
        //
        String _copy_from_path;
        String _copy_to_path;
        uint8_t _copy_progress = 0;
        //
        String _file_ext;
        String _dir_path;
        String _db_path;
        IndexMode _index_mode{INDX_MODE_DIR};

    private:
        const uint32_t TASK_SIZE{(1024 / 2) * 30};

        std::function<void()> _task_finished_callback = nullptr;

        bool startIndexTask(IndexMode mode, const char *dir_path, const char *db_path, std::function<void()> on_finish);
        //
        void rm();
        bool rmFile(const char *path);
        bool rmDir(const char *path);
        void copyFile();
        void index();
        //
        static void rmTask(void *params);
        static void copyFileTask(void *params);
        static void indexTask(void *params);
        //
        void doFinish();
    };
}