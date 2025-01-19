#pragma once
#include <Arduino.h>
#include <unordered_map>
#include "../util/bmp/BmpUtil.h"
#include "../util/wav/WavUtil.h"

namespace meow
{
    class ResManager
    {

    public:
        ~ResManager();

        /**
         * @brief Завантажує дані звукових доріжок до PSRAM.
         *
         * @param audio_descr Пари <id доріжки, шлях до доріжки на SD>.
         */
        void loadWavs(std::unordered_map<uint16_t, const char *> &audio_descr);

        /**
         * @brief Видаляє доріжку та її дані з пам'яті.
         *
         * @param id Ідентифікатор доріжки.
         */
        void deleteWav(uint16_t id);

        /*!
         * @brief
         *      Отримати вказівник на дані звукової доріжки
         * @param  id
         *
         * @return
         *
         */

        /**
         * @brief Повертає вказівник на дані звукової доріжки.
         *
         * @param id Ідентифікатор доріжки
         * @return WavData* - Вказівник на WavData, що містить дані про wav-файл. nullptr - якщо дані не знайдено
         */
        WavData *getWav(uint16_t id);

        // -------------------------------------------------------------------------------

        /**
         * @brief Завантажує дані BMP до PSRAM
         *
         * @param bmp_descr Пари <id BMP, шлях до BMP на SD>
         */
        void loadBmps(std::unordered_map<uint16_t, const char *> &bmp_descr);

        /**
         * @brief Видаляє BMP з пам'яті
         *
         * @param id Ідентифікатор даних BMP
         */
        void deleteBmp(uint16_t id);

        /**
         * @brief Повертає вказівник на дані BMP
         *
         * @param id Ідентифікатор завантаженого зображення
         * @return BmpData* - Вказівник на BmpData, що містить дані про bmp-файл. nullptr - якщо дані не знайдено
         */
        BmpData *getBmp(uint16_t id);

    private:
        std::unordered_map<uint16_t, WavData *> _wav_data;
        std::unordered_map<uint16_t, BmpData *> _bmp_data;
    };
}