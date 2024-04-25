#include "SokobanObj.h"

#include "../ClassID.h"
#include "../TriggerID.h"

#include "res/sprite/sprite_sokoban.h"

void SokobanObj::init()
{
    _class_ID = ClassID::CLASS_HERO; // Встановити ідентифікатор типу

    _sprite.img_ptr = SPRITE_SOKOBAN; // Встановити зображення спрайта
    _sprite.has_img = true;           // Указати, що об'єкт може малювати свій спрайт
    _sprite.width = 32;               // Ширина спрайта
    _sprite.height = 32;              // Висота спрайта

    _body.pass_abillity_mask |= Tile::TYPE_GROUND; // Маска типу прохідності ігрового об'єкта.
                                                   // Дозволяє обмежувати пересування об'єкта по певних видах плиток ігрової мапи
    initSprite();                                  // Ініціалізувати спрайт об'єкта
}

void SokobanObj::update()
{
}

IObjShape *SokobanObj::getShape()
{
    return nullptr;
}

void SokobanObj::reborn(IObjShape *shape)
{
}

void SokobanObj::onDraw()
{
    // Необовязковий метод
    // Якщо перевизначено, тут можна відрисувати все, що НЕ стосується спрайта об'єкта.
    // Наприклад, полоску XP над ним.

    IGameObject::onDraw(); // Необхідно обов'язково викликати бітьківський метод для відрисовки спрайту чи анімації.
}

void SokobanObj::move(MovingDirection direction)
{
    // TODO додати логіку переміщення ящиків
    // TODO додати перевірку переміщення

    if (direction == DIRECTION_UP)
    {
        _y_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_DOWN)
    {
        _y_global += PIX_PER_STEP;
    }
    else if (direction == DIRECTION_LEFT)
    {
        _x_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_RIGHT)
    {
        _x_global += PIX_PER_STEP;
    }
}
