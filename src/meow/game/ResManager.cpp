#include "ResManager.h"

namespace meow
{
    ResManager::~ResManager()
    {
        for (auto it = _wav_data.begin(), last_it = _wav_data.end(); it != last_it; ++it)
        {
            free((void *)(it->second->data_ptr));
            free(it->second);
        }

        for (auto it = _bmp_data.begin(), last_it = _bmp_data.end(); it != last_it; ++it)
        {
            if (it->second->is_loaded)
                free((void *)(it->second->data_ptr));

            free(it->second);
        }
    }

    void ResManager::loadWavs(std::unordered_map<uint16_t, const char *> audio_descr)
    {
        WavUtil util;
        for (auto it = audio_descr.begin(), last_it = audio_descr.end(); it != last_it; ++it)
        {
            try
            {
                if (_wav_data.at(it->first)) // запобігання оптимізації коду
                {
                    log_e("ID дублюється: %i", it->first);
                    continue;
                }
            }
            catch (std::out_of_range)
            {
                WavData data = util.loadWav(it->second);
                if (data.size > 0)
                {
                    WavData *ps_data = (WavData *)ps_malloc(sizeof(WavData));
                    if (!ps_data)
                    {
                        log_e("Помилка виділення пам'яті");
                        esp_restart();
                    }

                    ps_data->size = data.size;
                    ps_data->data_ptr = data.data_ptr;

                    _wav_data.insert(std::pair<uint16_t, WavData *>(it->first, ps_data));
                }
                else
                {
                    log_e("Помилка завантаження wav: %s", it->second);
                }
            }
        }
    }

    void ResManager::deleteWav(uint16_t id)
    {
        try
        {
            WavData *data = _wav_data.at(id);
            free(data->data_ptr);
            free(data);
            _wav_data.erase(id);
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
        }
    }

    WavData *ResManager::getWav(uint16_t id)
    {
        try
        {
            return _wav_data.at(id);
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
            return nullptr;
        }
    }
    //-----------------------------------------------------------------------------------

    void ResManager::loadBmps(std::unordered_map<uint16_t, const char *> bmp_descr)
    {
        BmpUtil util;
        for (auto it = bmp_descr.begin(), last_it = bmp_descr.end(); it != last_it; ++it)
        {
            try
            {
                if (_bmp_data.at(it->first))
                {
                    log_e("ID дублюється: %i", it->first);
                    continue;
                }
            }
            catch (std::out_of_range)
            {
                BmpData data = util.loadBmp(it->second);

                BmpData *ps_data = (BmpData *)ps_malloc(sizeof(BmpData));

                if (!ps_data)
                {
                    log_e("Помилка виділення пам'яті");
                    esp_restart();
                }

                ps_data->is_loaded = data.is_loaded;
                ps_data->width = data.width;
                ps_data->height = data.height;
                ps_data->data_ptr = data.data_ptr;

                _bmp_data.insert(std::pair<uint16_t, BmpData *>(it->first, ps_data));
            }
        }
    }

    void ResManager::deleteBmp(uint16_t id)
    {
        try
        {
            BmpData *data = _bmp_data.at(id);

            if (data->is_loaded)
                free((void *)data->data_ptr);

            free(data);

            _bmp_data.erase(id);
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
        }
    }

    BmpData *ResManager::getBmp(uint16_t id)
    {
        try
        {
            return _bmp_data.at(id);
        }
        catch (std::out_of_range)
        {
            log_e("Спроба звернутися до неіснуючого ключа: %i", id);
            return nullptr;
        }
    }
}
