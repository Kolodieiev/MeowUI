#include "BoxObj.h"
#include "../ClassID.h"

#include "./res/sprite_box.h"
#include "./res/sprite_box_ok.h"

void BoxObj::init()
{
    _class_ID = ClassID::CLASS_BOX;
    _sprite.img_ptr = SPRITE_BOX;
    _sprite.has_img = true;
    _sprite.width = 32;
    _sprite.height = 32;

    initSprite();
}

void BoxObj::update()
{

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
