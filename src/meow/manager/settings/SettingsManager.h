#pragma once
#include <Arduino.h>
#include "../files/FileManager.h"

namespace meow
{
    /**
     * @brief Дозволяє зберігати та завантажувати окремі налаштування до бінарних файлів на карту пам'яті.
     * Бінарні файли з налаштуваннями зберігаються у папці "/.data/preferences"
     *
     */
    class SettingsManager : private FileManager
    {
    public:
        using FileManager::isSdMounted;

        /**
         * @brief Зберігає налаштування до бінарного файлу.
         *
         * @param pref_name Ім'я налаштування.
         * @param value Значення налаштування.
         * @return true - якщо операція виконана успішно.
         * @return false - якщо операція завершилася невдачею.
         */
        bool set(const char *pref_name, const char *value);

        /**
         * @brief Читає налаштування з бінарного файлу.
         *
         * @param pref_name Ім'я налаштування.
         * @return String - Рядок, що містить дані налаштування, або порожній рядок, якщо файл з налаштуванням не вдалося прочитати.
         */
        String get(const char *pref_name);

    private:
        String getPrefFilePath(const char *pref_name);
    };
}