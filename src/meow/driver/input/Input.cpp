#include "Input.h"

namespace meow
{

    void Input::update()
    {
        for (auto &&btn : _buttons)
            btn.second->update();
    }

    void Input::reset()
    {
        for (auto &&btn : _buttons)
            btn.second->reset();
    }

}