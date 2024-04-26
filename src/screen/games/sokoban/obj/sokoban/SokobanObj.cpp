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
    std::list<IGameObject *> objs;   // Список повернутих об'єктів для наступного кроку
    std::list<IGameObject *> n_objs; // Список об'єктів на місці, куди повинен бути переміщений ящик

    if (direction == DIRECTION_UP)
    {
        objs = getObjInPoint(_x_global, _y_global - 1); // Вибрати всі об'єкти на плитці куди повинен здійснюватися наступний крок

        std::list<IGameObject *>::iterator it_i;
        for (it_i = objs.begin(); it_i != objs.end(); ++it_i)
        {
            if ((*it_i)->getClassID() == ClassID::CLASS_BOX) // Якщо знайдено об'єкт ящика
            {
                n_objs = getObjInPoint(_x_global, _y_global - 32 * 2); // Вибрати всі об'єкти на плитці куди повинен бути встановлений ящик

                std::list<IGameObject *>::iterator it_j;
                for (it_j = n_objs.begin(); it_j != n_objs.end(); ++it_j)
                {
                    if ((*it_j)->getClassID() == ClassID::CLASS_BOX) // Якщо знайдено об'єкт ящика, рух продовжувати не можна
                        return;
                }

                // Якщо ящик не знайдено, перевірити, чи не вприємося в стіну за ящиком
                if (!_game_map.canPass(_x_global, _y_global, _x_global, _y_global - PIX_PER_STEP * 2, _body, _sprite))
                {
                    // Якщо впираємося в стіну, рух продовжувати не можна
                    return;
                }

                // Якщо всі перевірки пройдено
                (*it_i)->_y_global -= PIX_PER_STEP; //  переміщуємо об'єкт ящика
                _y_global -= PIX_PER_STEP;          // переміщуємо об'єкт комірника, на одну клітинку
                return;
            }
        }

        // Якщо ящик не було знайдено, перевіряємо чи може комірник пройти
        if (_game_map.canPass(_x_global, _y_global, _x_global, _y_global - PIX_PER_STEP, _body, _sprite))
            _y_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_DOWN)
    {
        objs = getObjInPoint(_x_global, _y_global + PIX_PER_STEP);

        std::list<IGameObject *>::iterator it_i;
        for (it_i = objs.begin(); it_i != objs.end(); ++it_i)
        {
            if ((*it_i)->getClassID() == ClassID::CLASS_BOX)
            {
                n_objs = getObjInPoint(_x_global, _y_global + 32 * 2);

                std::list<IGameObject *>::iterator it_j;
                for (it_j = n_objs.begin(); it_j != n_objs.end(); ++it_j)
                {
                    if ((*it_j)->getClassID() == ClassID::CLASS_BOX)
                        return;
                }

                if (!_game_map.canPass(_x_global, _y_global, _x_global, _y_global + PIX_PER_STEP * 2, _body, _sprite))
                    return;

                (*it_i)->_y_global += PIX_PER_STEP;
                _y_global += PIX_PER_STEP;
                return;
            }
        }

        if (_game_map.canPass(_x_global, _y_global, _x_global, _y_global + PIX_PER_STEP, _body, _sprite))
            _y_global += PIX_PER_STEP;
    }
    else if (direction == DIRECTION_LEFT)
    {
        objs = getObjInPoint(_x_global - 1, _y_global);

        std::list<IGameObject *>::iterator it_i;
        for (it_i = objs.begin(); it_i != objs.end(); ++it_i)
        {
            if ((*it_i)->getClassID() == ClassID::CLASS_BOX)
            {
                n_objs = getObjInPoint(_x_global - 32 * 2, _y_global);

                std::list<IGameObject *>::iterator it_j;
                for (it_j = n_objs.begin(); it_j != n_objs.end(); ++it_j)
                {
                    if ((*it_j)->getClassID() == ClassID::CLASS_BOX)
                        return;
                }

                if (!_game_map.canPass(_x_global, _y_global, _x_global - PIX_PER_STEP * 2, _y_global, _body, _sprite))
                    return;

                (*it_i)->_x_global -= PIX_PER_STEP;
                _x_global -= PIX_PER_STEP;
                return;
            }
        }

        if (_game_map.canPass(_x_global, _y_global, _x_global - PIX_PER_STEP, _y_global, _body, _sprite))
            _x_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_RIGHT)
    {
        objs = getObjInPoint(_x_global + PIX_PER_STEP, _y_global);

        std::list<IGameObject *>::iterator it_i;
        for (it_i = objs.begin(); it_i != objs.end(); ++it_i)
        {
            if ((*it_i)->getClassID() == ClassID::CLASS_BOX)
            {
                n_objs = getObjInPoint(_x_global + 32 * 2, _y_global);

                std::list<IGameObject *>::iterator it_j;
                for (it_j = n_objs.begin(); it_j != n_objs.end(); ++it_j)
                {
                    if ((*it_j)->getClassID() == ClassID::CLASS_BOX)
                        return;
                }

                if (!_game_map.canPass(_x_global, _y_global, _x_global + PIX_PER_STEP * 2, _y_global, _body, _sprite))
                    return;

                (*it_i)->_x_global += PIX_PER_STEP;
                _x_global += PIX_PER_STEP;
                return;
            }
        }

        if (_game_map.canPass(_x_global, _y_global, _x_global + PIX_PER_STEP, _y_global, _body, _sprite))
            _x_global += PIX_PER_STEP;
    }
}

void SokobanObj::addBoxPtr(BoxObj *box_ptr)
{
    _boxes.push_back(box_ptr);
}
