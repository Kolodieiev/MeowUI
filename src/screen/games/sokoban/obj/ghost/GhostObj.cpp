#include "GhostObj.h"

void GhostObj::init()
{
    _body.is_rigid = false; // Вимикаємо тверде тіло для нашого привида, щоб він міг проходити крізь стіни
}

void GhostObj::update()
{
}

IObjShape *GhostObj::getShape()
{
    log_e("Об'єкт не повинен повертати свій образ");
    esp_restart();
    return nullptr;
}

void GhostObj::reborn(IObjShape *shape)
{
    log_e("Об'єкт не повинен бути відроджений");
    esp_restart();
}

void GhostObj::move(MovingDirection direction)
{
    if (direction == DIRECTION_UP)
    {
        // Даний об'єкт завжди може проходити по будь-якій плитці мапи, так як він не має твердого тіла.
        // Результат буде false тільки у випадку, якщо об'єкт спробує вийти за межі ігрового рівня.
        if (_game_map.canPass(_x_global, _y_global, _x_global, _y_global - PIX_PER_STEP, _body, _sprite))
            _y_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_DOWN)
    {
        if (_game_map.canPass(_x_global, _y_global, _x_global, _y_global + PIX_PER_STEP, _body, _sprite))
            _y_global += PIX_PER_STEP;
    }
    else if (direction == DIRECTION_LEFT)
    {
        if (_game_map.canPass(_x_global, _y_global, _x_global - PIX_PER_STEP, _y_global, _body, _sprite))
            _x_global -= PIX_PER_STEP;
    }
    else if (direction == DIRECTION_RIGHT)
    {
        if (_game_map.canPass(_x_global, _y_global, _x_global + PIX_PER_STEP, _y_global, _body, _sprite))
            _x_global += PIX_PER_STEP;
    }
}
