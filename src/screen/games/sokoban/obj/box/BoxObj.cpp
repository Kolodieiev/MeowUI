#include "BoxObj.h"
#include "../ClassID.h"

#include "./res/sprite_box.h"
#include "./res/sprite_box_ok.h"

void BoxObj::init()
{
    _class_ID = ClassID::CLASS_BOX;
    _layer = 1; // Об'єкт повинен бути вище об'єктів точок
    _sprite.img_ptr = SPRITE_BOX;
    _sprite.has_img = true;
    _sprite.width = 32;
    _sprite.height = 32;

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
