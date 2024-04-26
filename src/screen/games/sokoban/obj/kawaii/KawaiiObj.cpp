#include "KawaiiObj.h"
#include "./res/dance_0.h"
#include "./res/dance_1.h"
#include "./res/dance_2.h"
#include "./res/dance_3.h"
#include "./res/dance_4.h"
#include "./res/dance_5.h"
#include "./res/dance_6.h"
#include "./res/dance_7.h"
#include "./res/dance_8.h"
#include "./res/dance_9.h"
#include "./res/dance_10.h"
#include "./res/dance_11.h"
#include "./res/dance_12.h"
#include "./res/dance_13.h"
#include "./res/dance_14.h"
#include "./res/dance_15.h"
#include "./res/dance_16.h"
#include "./res/dance_17.h"
#include "./res/dance_18.h"
#include "./res/dance_19.h"
#include "./res/dance_20.h"
#include "./res/dance_21.h"
#include "./res/dance_22.h"
#include "./res/dance_23.h"
#include "./res/dance_24.h"
#include "./res/dance_25.h"

void KawaiiObj::init()
{
    _anim_dance.push_back(SPRITE_DANCE_0);
    _anim_dance.push_back(SPRITE_DANCE_1);
    _anim_dance.push_back(SPRITE_DANCE_2);
    _anim_dance.push_back(SPRITE_DANCE_3);
    _anim_dance.push_back(SPRITE_DANCE_4);
    _anim_dance.push_back(SPRITE_DANCE_5);
    _anim_dance.push_back(SPRITE_DANCE_6);
    _anim_dance.push_back(SPRITE_DANCE_7);
    _anim_dance.push_back(SPRITE_DANCE_8);
    _anim_dance.push_back(SPRITE_DANCE_9);
    _anim_dance.push_back(SPRITE_DANCE_10);
    _anim_dance.push_back(SPRITE_DANCE_11);
    _anim_dance.push_back(SPRITE_DANCE_12);
    _anim_dance.push_back(SPRITE_DANCE_13);
    _anim_dance.push_back(SPRITE_DANCE_14);
    _anim_dance.push_back(SPRITE_DANCE_15);
    _anim_dance.push_back(SPRITE_DANCE_16);
    _anim_dance.push_back(SPRITE_DANCE_17);
    _anim_dance.push_back(SPRITE_DANCE_18);
    _anim_dance.push_back(SPRITE_DANCE_19);
    _anim_dance.push_back(SPRITE_DANCE_20);
    _anim_dance.push_back(SPRITE_DANCE_21);
    _anim_dance.push_back(SPRITE_DANCE_22);
    _anim_dance.push_back(SPRITE_DANCE_23);
    _anim_dance.push_back(SPRITE_DANCE_24);
    _anim_dance.push_back(SPRITE_DANCE_25);

    _sprite.width = 173;
    _sprite.height = 240;

    _sprite.has_animation = true;

    _sprite.frames_between_anim = 2;
    _sprite.cur_anim = &_anim_dance;
    initSprite();
}

void KawaiiObj::update()
{
}

IObjShape *KawaiiObj::getShape()
{
    return nullptr;
}

void KawaiiObj::reborn(IObjShape *shape)
{
}
