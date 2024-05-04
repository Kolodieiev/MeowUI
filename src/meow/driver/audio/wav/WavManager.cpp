#include "WavManager.h"
#include "driver/i2s.h"

namespace meow
{

    uint64_t WavManager::_track_id;
    std::map<uint16_t, WavTrack *> WavManager::_mix;
    std::map<uint16_t, WavTrack *> WavManager::_preloaded_tracks;

    WavManager::~WavManager()
    {
        if (_task_handle != NULL)
        {
            _params.cmd = TaskParams::CMD_STOP;
            vTaskDelete(_task_handle);
        }

        if (_is_inited)
            i2s_driver_uninstall(I2S_NUM_0);

        std::map<uint16_t, WavTrack *>::iterator it;
        for (it = _preloaded_tracks.begin(); it != _preloaded_tracks.end(); ++it)
        {
            it->second->freeData();
            delete it->second;
        }

        std::map<uint16_t, WavTrack *>::iterator mix_it;
        for (mix_it = _mix.begin(); mix_it != _mix.end(); ++mix_it)
        {
            delete mix_it->second;
        }

        _preloaded_tracks.clear();
        _mix.clear();
        _track_id = 0;
    }

    bool WavManager::init(uint8_t I2S_BCLK, uint8_t I2S_LRC, uint8_t I2S_DOUT)
    {
        i2s_config_t i2s_config = {};

        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
        i2s_config.sample_rate = 16000;
        i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
        i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
        i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL4;
        i2s_config.dma_buf_count = 4;
        i2s_config.dma_buf_len = 1024;
        i2s_config.use_apll = false;
        i2s_config.fixed_mclk = true;
        i2s_config.tx_desc_auto_clear = true;

        if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK)
        {
            log_e("Помилка ініціалізації I2S драйверу");
            return false;
        }

        i2s_pin_config_t pin_config = {};

        pin_config.bck_io_num = I2S_BCLK;
        pin_config.ws_io_num = I2S_LRC;
        pin_config.data_out_num = I2S_DOUT;
        pin_config.data_in_num = I2S_PIN_NO_CHANGE;
        pin_config.mck_io_num = I2S_PIN_NO_CHANGE;

        if (i2s_set_pin(I2S_NUM_0, &pin_config) != ESP_OK)
        {
            log_e("Помилка ініціалізації I2S пінів");
            return false;
        }

        _is_inited = true;
        return true;
    }

    void WavManager::addToPreloaded(uint16_t id, WavTrack *sound)
    {
        if (!sound)
        {
            log_e("Помилка. sound == null");
            esp_restart();
        }

        try
        {
            WavTrack *sound = _preloaded_tracks.at(id);
            log_e("Спроба замінити існуючу пару: %i", id);
        }
        catch (std::out_of_range)
        {
            _preloaded_tracks.insert(_preloaded_tracks.end(), std::pair<uint16_t, WavTrack *>(id, sound));
        }
    }

    void WavManager::removeFromPreloaded(uint16_t id)
    {
        try
        {
            WavTrack *sound = _preloaded_tracks.at(id);
            sound->freeData();
            delete sound;
            _preloaded_tracks.erase(id);
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
        }
    }

    WavTrack *WavManager::getPreloadedByID(uint16_t id)
    {
        try
        {
            WavTrack *sound = _preloaded_tracks.at(id);
            return sound->clone();
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
            return nullptr;
        }
    }

    uint16_t WavManager::addToMix(WavTrack *sound)
    {
        if (!sound)
        {
            log_e("Помилка. sound == null");
            esp_restart();
        }

        ++_track_id;
        _mix.insert(_mix.end(), std::pair<uint16_t, WavTrack *>(_track_id, sound));
        return _track_id;
    }

    void WavManager::removeFromMix(uint16_t id)
    {
        _mix.erase(id);
    }

    void WavManager::startMix()
    {
        if (!_is_inited)
        {
            log_e("Помилка. I2S не ініціалізовано");
            return;
        }

        if (_is_playing)
            return;

        _is_playing = true;

        xTaskCreatePinnedToCore(mixTask, "mixTask", (1024 / 2) * 10, &_params, 10, &_task_handle, 0);
    }

    void WavManager::pauseResume()
    {
        if (_params.cmd == TaskParams::CMD_NONE)
        {
            _params.cmd = TaskParams::CMD_PAUSE;
        }
        else if (_params.cmd == TaskParams::CMD_PAUSE)
        {
            _params.cmd = TaskParams::CMD_NONE;
        }
    }

    void WavManager::mixTask(void *params)
    {
        const int16_t I2S_BUF_LEN{1024};
        uint8_t _samples_buf[2048];

        TaskParams *task_params = (TaskParams *)params;

        int16_t sample;
        std::map<uint16_t, WavTrack *>::iterator it;
        size_t bytes_written;

        while (task_params->cmd != TaskParams::CMD_STOP)
        {
            if (task_params->cmd != TaskParams::CMD_PAUSE)
            {
                for (int16_t i{0}; i < I2S_BUF_LEN; i += 2)
                {
                    sample = 0;

                    for (it = _mix.begin(); it != _mix.end(); ++it)
                    {
                        if (it->second->isPlaying())
                        {
                            // TODO clipping
                            sample += it->second->getNextSample();
                        }
                        else
                        {
                            delete it->second;
                            _mix.erase(it);
                        }
                    }

                    _samples_buf[i + 1] = sample;
                }

                i2s_write(I2S_NUM_0, _samples_buf, I2S_BUF_LEN, &bytes_written, 1500 / portTICK_PERIOD_MS);
            }

            vTaskDelay(1 / portTICK_PERIOD_MS);
        }

        vTaskDelete(NULL);
    }

}