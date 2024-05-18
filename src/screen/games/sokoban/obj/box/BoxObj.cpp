#include "BoxObj.h"
#include "../ClassID.h"

#include "./res/sprite_box.h"
#include "./res/sprite_box_ok.h"

#include "../../ResID.h"

void BoxObj::init()
{
    _class_ID = ClassID::CLASS_BOX;
    _layer = 1; // Об'єкт повинен бути вище об'єктів точок
    _sprite.img_ptr = SPRITE_BOX;
    _sprite.has_img = true;
    _sprite.width = 32;
    _sprite.height = 32;
    _body.pass_abillity_mask = Tile::TYPE_GROUND;

    initSprite();
}

void BoxObj::update()
{
    _is_ok = false; // Необхідно на випадок, якщо ящик було переміщено із ключової точки
    _sprite.img_ptr = SPRITE_BOX;

    std::list<IGameObject *> objs = getObjInPoint(_x_global, _y_global); // Отримати список усіх об'єктів у точці

    std::list<IGameObject *>::iterator it;
    for (it = objs.begin(); it != objs.end(); ++it)
    {
        if ((*it)->getClassID() == ClassID::CLASS_BOX_POINT) // Якщо об'єкт належить до типу BoxPointObj
        {
            _is_ok = true;                   // Підняти прапор, який вказує, що ящик встановлено в потрібному місці
            _sprite.img_ptr = SPRITE_BOX_OK; // Змінити спрайт об'єкта
            break;
        }
    }
}

IObjShape *BoxObj::getShape()
{
    log_e("Об'єкт не повинен повертати свій образ");
    esp_restart();
    return nullptr;
}

void BoxObj::reborn(IObjShape *shape)
{
    log_e("Об'єкт не повинен бути відновлений");
    esp_restart();
}

bool BoxObj::moveTo(uint16_t x, uint16_t y)
{
    std::list<IGameObject *> objs = getObjInPoint(x, y); // Вибрати всі об'єкти на плитці куди повинен бути встановлений ящик

    std::list<IGameObject *>::iterator it_j;
    for (it_j = objs.begin(); it_j != objs.end(); ++it_j)
    {
        if ((*it_j)->getClassID() == ClassID::CLASS_BOX) // Якщо знайдено об'єкт ящика, рух продовжувати не можна
            return false;
    }

    // Якщо ящик не знайдено, перевірити, чи не вприємося в стіну за ящиком
    if (!_game_map.canPass(_x_global, _y_global, x, y, _body, _sprite))
    {
        // Якщо впираємося в стіну, рух продовжувати не можна
        return false;
    }

    // Якщо всі перевірки пройдено
    _x_global = x; //  переміщуємо об'єкт ящика
    _y_global = y;

    // Відтворення звуку по його ідентфікатору

    int start = 0;
    int end = 4;

    // Отримати випадковий звук переміщення
    uint8_t track_ID = rand() % (end - start + 1) + start;
    WavData *s_data = _res.getWav(track_ID);

    if (s_data && s_data->size > 0)
    {
        WavTrack *track = new WavTrack(s_data->data_ptr, s_data->size);

        if (!track)
        {
            log_e("Помилка виділення пам'яті");
            esp_restart();
        }

        _audio.addToMix(track);
    }
    else
    {
        log_e("Звукові дані з таким ID{%d} відсутні", track_ID);
    }

    return true;
}
