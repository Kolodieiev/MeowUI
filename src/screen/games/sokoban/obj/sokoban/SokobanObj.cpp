#include "SokobanObj.h"

#include "../ClassID.h"
#include "../TriggerID.h"

#include "res/sprite/sprite_sokoban.h"

void SokobanObj::init()
{
    _class_ID = ClassID::CLASS_HERO;  // Встановити ідентифікатор типу
    _layer = 1;                       // Об'єкт повинен бути вище об'єктів точок
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
    bool done{true};

    for (uint8_t i{0}; i < _boxes.size(); ++i)
    {
        if (!_boxes[i]->isOk())
        {
            done = false;
            break;
        }
    }

    if (done)
    {
        _trigger_ID = TriggerID::TRIGGER_NEXT_SCENE;
        _is_triggered = true;
    }
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
    if (direction == DIRECTION_UP)
        stepTo(_x_global, _y_global - PIX_PER_STEP, _x_global, _y_global - PIX_PER_STEP * 2);
    else if (direction == DIRECTION_DOWN)
        stepTo(_x_global, _y_global + PIX_PER_STEP, _x_global, _y_global + PIX_PER_STEP * 2);
    else if (direction == DIRECTION_LEFT)
        stepTo(_x_global - PIX_PER_STEP, _y_global, _x_global - PIX_PER_STEP * 2, _y_global);
    else if (direction == DIRECTION_RIGHT)
        stepTo(_x_global + PIX_PER_STEP, _y_global, _x_global + PIX_PER_STEP * 2, _y_global);
}

void SokobanObj::addBoxPtr(BoxObj *box_ptr)
{
    _boxes.push_back(box_ptr);
}

void SokobanObj::stepTo(uint16_t x, uint16_t y, uint16_t box_x_step, uint16_t box_y_step)
{
    std::list<IGameObject *> objs = getObjInPoint(x, y); // Вибрати всі об'єкти на плитці куди повинен здійснюватися наступний крок

    std::list<IGameObject *>::iterator it_i;
    for (it_i = objs.begin(); it_i != objs.end(); ++it_i)
    {
        if ((*it_i)->getClassID() == ClassID::CLASS_BOX) // Якщо знайдено об'єкт ящика
        {
            BoxObj *box = (BoxObj *)*it_i;

            if (box->moveTo(box_x_step, box_y_step)) // намагаємося посунути ящик
            {
                _x_global = x; // Якщо успішно, рухаємось за ящиком
                _y_global = y;
            }

            return;
        }
    }

    // Якщо ящик не було знайдено, перевіряємо чи може комірник пройти
    if (_game_map.canPass(_x_global, _y_global, x, y, _body, _sprite))
    {
        _x_global = x; // Якщо перевірка успішна - рухаємо комірника
        _y_global = y;
    }
}
