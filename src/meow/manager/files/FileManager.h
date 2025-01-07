#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <Stream.h>

namespace meow
{
    /**
     * @brief Структура, яка зберігає мінімальну інформацію про ВказівникНаФайл, а саме його ім'я та чи є ВказівникНаФайл папкою.
     *
     */
    struct FileInfo
    {
    public:
        FileInfo(const String &name, bool is_dir);
        ~FileInfo() { free(_name); }

        /**
         * @brief Повертає стан прапора, який було встановлено під час створення об'єкта, та який вказує чи є ВказівникНаФайл папкою.
         *
         * @return true - якщо ВказівникНаФайл вказує на папку.
         * @return false - якщо ВказівникНаФайл вказує на бінарний файл.
         */
        bool isDir() const { return _is_dir; }

        /**
         * @brief Повертає ім'я об'єкта, на який вказує ВказівникНаФайл.
         *
         * @return const char* - вказівник на масив символів, що містить ім'я об'єкта.
         */
        const char *getName() const { return _name; }
        //
        bool operator<(const FileInfo &other) const
        {
            if (_is_dir != other._is_dir)
                return _is_dir;

            const char *lhs = _name;
            const char *rhs = other._name;

            while (*lhs && *rhs)
            {
                if (std::isdigit(*lhs) && std::isdigit(*rhs))
                {
                    char *end_lhs;
                    char *end_rhs;
                    long num_lhs = std::strtol(lhs, &end_lhs, 10);
                    long num_rhs = std::strtol(rhs, &end_rhs, 10);

                    if (num_lhs != num_rhs)
                    {
                        return num_lhs < num_rhs;
                    }
                    lhs = end_lhs;
                    rhs = end_rhs;
                }
                else
                {
                    if (*lhs != *rhs)
                    {
                        return *lhs < *rhs;
                    }
                    ++lhs;
                    ++rhs;
                }
            }

            return std::strcmp(lhs, rhs) < 0;
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

    /**
     * @brief Тип, що перевантажує основні методи класу Stream. Може бути використаний у випадках,
     * коли методи класів Arduino вимагають об'єкт успадкований від Stream.
     *
     */
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

    /**
     * @brief
     *
     */
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
         * @param out_path Змінна куди буди записано повний шлях.
         * @param path Шлях, вказаний без точки монтування.
         */
        void makeFullPath(String &out_path, const char *path);

        /**
         * @brief Перевіряє чи існує двійковий файл на карті пам'яті.
         *
         * @param path Шлях до файлу, вказаний без точки монтування
         * @param silently Прапор, який вказує, чи потрібно пропустити вивід повідомлення в консоль, якщо файл відсутній.
         * @return true - якщо файл існує.
         * @return false - якщо файл не існує або за вказаним шляхом знаходиться директорія.
         */
        bool fileExist(const char *path, bool silently = false);

        /**
         * @brief Перевіряє чи існує папка на карті пам'яті.
         *
         * @param path Шлях, вказаний без точки монтування.
         * @param silently Прапор, який вказує, чи потрібно пропустити вивід повідомлення в консоль, якщо папка відсутня.
         * @return true - якщо папка існує.
         * @return false - якщо папка не існує або за вказаним шляхом знаходиться двійковий файл.
         */
        bool dirExist(const char *path, bool silently = false);

        /**
         * @brief Перевіряє чи існує ВказівникНаФайл на карті пам'яті за вказаним шляхом.
         *
         * @param path Шлях, вказаний без точки монтування.
         * @param silently Прапор, який вказує, чи потрібно пропустити вивід повідомлення в консоль, якщо ВказівникНаФайл не виявлено.
         * @return true - якщо ВказівникНаФайл існує.
         * @return false - якщо ВказівникНаФайл не існує.
         */
        bool exists(const char *path, bool silently = false);

        /**
         * @brief Повертає розмір двійкового файлу за вказаним шляхом.
         *
         * @param path Шлях, вказаний без точки монтування.
         * @return size_t - Розмір двійкового файлу в байтах. 0 - Якщо двійковий файл не існує.
         */
        size_t getFileSize(const char *path);

        /**
         * @brief Читає метадані ВказівникаНаФайл, що знаходиться за вказаним шляхом.
         *
         * @param out_stat Адреса об'єкта, в який будуть записані метадані про ВказівникНаФайл, якщо він існує.
         * @param path  Шлях до ВказівникаНаФайл, вказаний без точки монтування.
         * @return true - Якщо ВказівникНаФайл існує за вказаним шляхом та метадані було прочитано.
         * @return false - Якщо ВказівникНаФайл не існує або не вдалося прочитати його метадані.
         */
        bool readStat(struct stat &out_stat, const char *path);

        /**
         * @brief Індексує тільки бінарні файли з указаним розширенням у вказаній папці без обходу вкладених директорій.
         *
         * @param out_vec Адреса об'єкта вектора, в який буде записано інформацію про виявленні бінарні файли.
         * @param dir_path Шлях до папки, в якій повинна бути виконана індексація, вказаний без точки монтування.
         * @param file_ext Розширення бінарних файлів, які повинні бути проіндексовані.
         */
        void indexFilesExt(std::vector<FileInfo> &out_vec, const char *dir_path, const char *file_ext);

        /**
         * @brief Індексує усі бінарні файли у вказаній папці без обходу вкладених директорій.
         *
         * @param out_vec Адреса об'єкта вектора, в який буде записано інформацію про виявленні бінарні файли.
         * @param dir_path Шлях до папки, в якій повинна бути виконана індексація, вказаний без точки монтування.
         */
        void indexFiles(std::vector<FileInfo> &out_vec, const char *dir_path);

        /**
         * @brief Індексує усі папки у вказаній папці без обходу вкладених директорій.
         *
         * @param out_vec Адреса об'єкта вектора, в який буде записано інформацію про папки.
         * @param dir_path Шлях до папки, в якій повинна бути виконана індексація, вказаний без точки монтування.
         */
        void indexDirs(std::vector<FileInfo> &out_vec, const char *dir_path);

        /**
         * @brief Індексує усі папки та бінарні файли у вказаній папці без обходу вкладених директорій.
         *
         * @param out_vec Адреса об'єкта вектора, в який буде записано інформацію про папки та бінарні файли.
         * @param dir_path Шлях до папки, в якій повинна бути виконана індексація, вказаний без точки монтування.
         */
        void indexAll(std::vector<FileInfo> &out_vec, const char *dir_path);

        /**
         * @brief Намагається створити та запустити задачу FreeRTOS, в процесі роботи якої, буде виконана спроба
         * видалити бінарний файл або папку за вказаним шляхом.
         *
         * @param path Шлях до ВказівникаНаФайл, вказаний без точки монтування.
         * @return true - якщо задачу було успішно створено та запущено.
         * @return false - якщо вже працює будь-яка інша задача файлового менеджера або не вдалося створити нову задачу за будь-якої причини.
         */
        bool startRemoving(const char *path);

        /**
         * @brief Намагається створити та запустити задачу FreeRTOS, в процесі роботи якої буде виконана спроба створити копію вказаного бінарного файлу.
         *
         * @param from Шлях до вказівника на бінарний файл, вказаний без точки монтування, з якого буде виконана спроба створити копію.
         * @param to Шлях до бінарного файла, вказаний без точки монтування, куди буде виконана спроба створити копію.
         * @return true - якщо задачу було успішно створено та запущено.
         * @return false - якщо вже працює будь-яка інша задача файлового менеджера або не вдалося створити нову задачу за будь-якої причини.
         */
        bool startCopyFile(const char *from, const char *to);

        /**
         * @brief Відміняє будь-яку запущену поточну задачу файлового менеджера.
         *
         */
        void cancel();

        /**
         * @brief Перейменовує ВказівникНаФайл.
         *
         * @param old_name Старий шлях до ВказівникНаФайл, вказаний без точки монтування.
         * @param new_name Новий шлях до ВказівникНаФайл, вказаний без точки монтування.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool rename(const char *old_name, const char *new_name);

        /**
         * @brief Створює папку з указаним іменем.
         *
         * @param path Ім'я папки, вказане без точки монтування.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool createDir(const char *path);

        /**
         * @brief Видаляє бінарний файл в контексті поточної задачі.
         *
         * @param path Шлях до файла.
         * @param make_full Прапор, який вказує, чи потрібно додати точку монтування до вказаного шляху.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool rmFile(const char *path, bool make_full = false);

        /**
         * @brief Видаляє папку в контексті поточної задачі.
         *
         * @param path Шлях до папки.
         * @param make_full Прапор, який вказує, чи потрібно додати точку монтування до вказаного шляху.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool rmDir(const char *path, bool make_full = false);

        /**
         * @brief Читає побайтово бінарний файл за вказаним шляхом.
         *
         * @param out_buffer Вихідний буфер, куди будуть записані прочитані байти.
         * @param path Шлях до бінарного файла, вказаний без точки монтування.
         * @param len Кількість байт, які повинні бути прочитані.
         * @param seek_pos Позиція від початку бінарного файла у байтах, з якої необхідно розпочати читання.
         * @return size_t - кількість успішно прочитаних байт. 0 - Якщо операція завершилася невдачею за будь-якої причини.
         */
        size_t readFile(char *out_buffer, const char *path, size_t len, int32_t seek_pos = 0);

        /**
         * @brief Читає вказану кількість байт з відкритого бінарного файла.
         *
         * @param file Вказівник на відкрити бінарний файл.
         * @param out_buffer Вихідний буфер, куди будуть записані прочитані байти.
         * @param len Кількість байт, які повинні бути прочитані.
         * @param seek_pos Позиція від початку бінарного файла у байтах, з якої необхідно розпочати читання.
         * Якщо 0 - читає з поточної.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool readFromFile(FILE *file, void *out_buffer, size_t len = 1, int32_t seek_pos = 0);

        /**
         * @brief Створює або перезаписує бінарний файл, та записує до нього вказану кількість байт із буфера.
         * Байти записуються блоками, якщо це можливо, для більш швидкого виконання операції.
         *
         * @param path Шлях до бінарного файла, вказаний без точки монтування.
         * @param buffer Буфер, з якого байти будуть записані до бінарного файлу.
         * @param len Кількість байт, які повинні бути записані до файлу.
         * @return size_t - кількість успішно записаних байт. 0 - якщо операція завершилася невдачею.
         */
        size_t writeFile(const char *path, const void *buffer, size_t len);

        /**
         * @brief Записує вказану кількість байт до відкритого бінарного файлу.
         * Байти записуються блоками, якщо це можливо, для більш швидкого виконання операції.
         *
         * @param file Вказівник на відкрити бінарний файл.
         * @param buffer Буфер, з якого байти будуть записані до бінарного файлу.
         * @param len Кількість байт, які повинні бути записані до файлу.
         * @return size_t - кількість успішно записаних байт. 0 - якщо операція завершилася невдачею.
         */
        size_t writeToFile(FILE *file, const void *buffer, size_t len);

        /**
         * @brief Повертає дескриптор бінарного файла, який було відкрито зі вказаним режимом.
         *
         * @param path Шлях до бінарного файла, вказаний без точки монтування.
         * @param mode Режим відкриття бінарного файла.
         * @return FILE* - вказівник на відкритий бінарний файл. NULL - якщо операція завершилася невдачею.
         */
        FILE *getFileDescriptor(const char *path, const char *mode);

        /**
         * @brief Закриває відкритий бінарний файл.
         *
         * @param file Посилання на вказівник на відкритий бінарний файл.
         */
        void closeFile(FILE *&file);

        /**
         * @brief Переміщує каретку у відкритому бінарному файлі.
         *
         * @param file Вказівник на відкритий бінарний файл.
         * @param pos Позиція у бінарному файлі, куди повинна бути переміщена каретка.
         * @param mode Режим у якому буде переміщена каретка. SEEK_SET; SEEK_CUR; SEEK_END.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool seekPos(FILE *file, int32_t pos, uint8_t mode = SEEK_SET);

        /**
         * @brief Повертає поточну позицію каретки у відкритому бінарному файлі.
         *
         * @param file Вказівник на відкрити бінарний файл.
         * @return size_t - позиція картки. 0 - якщо операція завершилася невдачею або каретка ще не була переміщена.
         */
        size_t getPos(FILE *file);

        /**
         * @brief Повертає кількість байт, які лишилися до кінця бінарного файла від поточної позиції каретки.
         *
         * @param file_size Розмір файла у байтах.
         * @param file Вказівник на відкрити бінарний файл.
         * @return size_t - кількість байт, які лишилися до кінця бінарного файла від поточної позиції каретки. 0 -  якщо операція завершилася невдачею.
         */
        size_t available(size_t file_size, FILE *file);

        /**
         * @brief Повертає прогрес операції копіювання бінарного файла.
         *
         * @return uint8_t - прогрес операції копіювання бінарного файла у відсотках.
         */
        uint8_t getCopyProgress() const { return _copy_progress; }

        /**
         * @brief Встановлює обробник події завершення будь-якої задачі.
         *
         * @param handler Обробник події завершення операції.
         * @param arg Аргументи, які будуть повернуті до обробника.
         */
        void setTaskDoneHandler(TaskDoneHandler handler, void *arg);

        /**
         * @brief Повертає стан прапору, який вказує на те, чи запущена в даний момент будь-яка із задач файлового менеджера.
         *
         * @return true - якщо будь-яка із задач файлового менеджера запущена на даний момент.
         * @return false - якщо жодна із задач файлового менеджера наразі не запущена.
         */
        bool isWorking() const { return _is_working; }

    private:
        enum IndexMode : uint8_t
        {
            INDX_MODE_DIR = 0,
            INDX_MODE_FILES,
            INDX_MODE_FILES_EXT,
            INDX_MODE_ALL
        };

        const uint32_t TASK_SIZE{(1024 / 2) * 30};
        //
        String _rm_path;
        //
        String _copy_from_path;
        String _copy_to_path;
        uint8_t _copy_progress{0};
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