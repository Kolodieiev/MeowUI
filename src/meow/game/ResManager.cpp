#include "ResManager.h"

namespace meow
{
    ResManager::~ResManager()
    {
        std::unordered_map<uint16_t, WavData *>::iterator audio_data_it;
        for (audio_data_it = _wav_data.begin(); audio_data_it != _wav_data.end(); ++audio_data_it)
        {
            free((void *)(audio_data_it->second->data_ptr));
            free(audio_data_it->second);
        }

        std::unordered_map<uint16_t, BmpData *>::iterator bmp_data_it;
        for (bmp_data_it = _bmp_data.begin(); bmp_data_it != _bmp_data.end(); ++bmp_data_it)
        {
            if (bmp_data_it->second->is_loaded)
                free((void *)(bmp_data_it->second->data_ptr));

            free(bmp_data_it->second);
        }
    }

    void ResManager::loadWavs(std::unordered_map<uint16_t, const char *> audio_descr)
    {
        WavUtil util;
        std::unordered_map<uint16_t, const char *>::iterator audio_descr_it;
        for (audio_descr_it = audio_descr.begin(); audio_descr_it != audio_descr.end(); ++audio_descr_it)
        {
            try
            {
                if (_wav_data.at(audio_descr_it->first)) // запобігання оптимізації коду
                {
                    log_e("ID дублюється: %i", audio_descr_it->first);
                    continue;
                }
            }
            catch (std::out_of_range)
            {
                WavData data = util.loadWav(audio_descr_it->second);
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

                    _wav_data.insert(std::pair<uint16_t, WavData *>(audio_descr_it->first, ps_data));
                }
                else
                {
                    log_e("Помилка завантаження wav: %s", audio_descr_it->second);
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
        std::unordered_map<uint16_t, const char *>::iterator bmp_descr_it;
        for (bmp_descr_it = bmp_descr.begin(); bmp_descr_it != bmp_descr.end(); ++bmp_descr_it)
        {
            try
            {
                if (_bmp_data.at(bmp_descr_it->first))
                {
                    log_e("ID дублюється: %i", bmp_descr_it->first);
                    continue;
                }
            }
            catch (std::out_of_range)
            {
                BmpData data = util.loadBmp(bmp_descr_it->second);

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

                _bmp_data.insert(std::pair<uint16_t, BmpData *>(bmp_descr_it->first, ps_data));
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
