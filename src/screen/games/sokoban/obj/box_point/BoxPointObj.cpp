#include "BoxPointObj.h"
//
#include "../ClassID.h"
//
#include "./res/sprite_box_point.h"

void BoxPointObj::init()
{
    _class_ID = ClassID::CLASS_BOX_POINT;
    _sprite.img_ptr = SPRITE_BOX_POIN;
    _sprite.has_img = true;
    _sprite.width = 32;
    _sprite.height = 32;

    _body.is_rigid = false;

    initSprite();
}

void BoxPointObj::update()
{
}

IObjShape *BoxPointObj::getShape()
{
    log_e("Об'єкт не повинен повертати свій образ");
    esp_restart();
    return nullptr;
}

void BoxPointObj::reborn(IObjShape *shape)
{
    log_e("Об'єкт не повинен бути відновлений");
    esp_restart();
}
