#pragma once
// #pragma GCC optimize("Ofast")
#include <Arduino.h>
#include <list>
#include <map>
#include "WavTrack.h"

namespace meow
{

#define PIN_I2S_BCLK 42
#define PIN_I2S_LRC 1
#define PIN_I2S_DOUT 2

    class WavManager
    {
    public:
        ~WavManager();

        bool init(uint8_t I2S_BCLK = PIN_I2S_BCLK, uint8_t I2S_LRC = PIN_I2S_LRC, uint8_t I2S_DOUT = PIN_I2S_DOUT);

        /*!
         * @brief
         *       Додати передзавантажену звукову доріжку. Ресурси зі списку передзавантажених звільняються автоматично.
         * @param  id
         *      Ідентифікатор передзавантаженої доріжки, по якому її можна буде знайти в списку.
         * @param  track
         *      Вказівник на звукову доріжку.
         */
        void addToPreloaded(uint16_t id, WavTrack *track);

        /*!
         * @brief
         *       Видалити звукову доріжку із передзавантажених.
         * @param  id
         *      Ідентифікатор передзавантаженої доріжки.
         */
        void removeFromPreloaded(uint16_t id);

        /*!
         * @brief
         *      Отримати копію звукової доріжки зі списка завантажених.
         * @param  id
         *      Ідентифікатор передзавантаженої доріжки.
         * @return
         *      Вказівник на копію доріжки, що раніше було збережено до передзавантажених. Якщо доріжку не знайдено - nullptr.
         */
        WavTrack *getPreloadedByID(uint16_t id);

        /*!
         * @brief
         *      Додати звукову доріжку до списку міксування. Ресурси зі списку міксування звільняються автоматично.
         * @param  track
         *      Вказівник на передзавантажену доріжку.
         * @return
         *      Ідентифікатор на доріжку в списку міксування.
         */
        uint16_t addToMix(WavTrack *track);

        /*!
         * @brief
         *     Видалити доріжку із списку міксування.
         * @param  id
         *     Ідентифікатор, який було присвоєно доріжці, під час додавання до міксу.
         */
        void removeFromMix(uint16_t id);

        /*!
         * @brief
         *     Стартувати задачу відтворення міксу.
         */
        void startMix();

        /*!
         * @brief
         *    Поставити на паузу/відновити вітворення міксу.
         */
        void pauseResume();

    private:
        bool _is_inited{false};
        bool _is_playing{false};

        static uint64_t _track_id;
        static std::map<uint16_t, WavTrack *> _mix;
        static std::map<uint16_t, WavTrack *> _preloaded_tracks;

        struct TaskParams
        {
            enum CMD : uint8_t
            {
                CMD_NONE = 0,
                CMD_PAUSE,
                CMD_STOP
            };

            CMD cmd{CMD_NONE};
        } _params;

        TaskHandle_t _task_handle = NULL;

        static void mixTask(void *params);
    };

}