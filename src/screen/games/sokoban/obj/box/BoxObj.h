#pragma once
#include <Arduino.h>
#include "meow/game/object/IGameObject.h"

using namespace meow;

class BoxObj : public IGameObject
{
public:
    BoxObj(GraphicsDriver &display,
           ResManager &res,
           WavManager &audio,
           GameMap &game_map,
           std::list<IGameObject *> &game_objs) : IGameObject(display, res, audio, game_map, game_objs) {}
    virtual ~BoxObj() {}
    virtual void init() override;
    virtual void update() override;
    virtual IObjShape *getShape() override;
    virtual void reborn(IObjShape *shape) override;

    // Якщо можливо, переміститися в задані координати. Повертає true в разі успіху
    bool moveTo(uint16_t x, uint16_t y);

    inline bool isOk() const { return _is_ok; }

private:
    bool _is_ok{false}; // Вказує, чи встановлено ящик в ключову точку
};