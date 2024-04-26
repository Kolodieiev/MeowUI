#pragma once
#include <Arduino.h>

#include "meow/game/IGameScene.h"
#include "../obj/ghost/GhostObj.h"
#include "../obj/sokoban/SokobanObj.h"

using namespace meow;

class Level1Scene : public IGameScene
{

public:
    Level1Scene(GraphicsDriver &display, Input &input, std::vector<IObjShape *> &stored_objs, bool is_loaded = false);
    virtual ~Level1Scene();
    virtual void update() override;
    virtual void onTriggered(int16_t id) override;

private:
    const uint8_t BOX_NUM{20};

    GhostObj *_ghost;     
    SokobanObj *_sokoban; 

    bool _is_ghost_selected{true}; 

    void buildMap();
    void createGhost();
    void createSokoban();
    void createBoxes();
    void createBoxPoints();
};
