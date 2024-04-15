/*
 * Генератор віджетів.
 * Використовується тут для часткового очищення файлів вікон від однотипного коду.
 */

#pragma once
#include <Arduino.h>
#include "meow/driver/graphics/GraphicsDriver.h"

#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/navbar/NavBar.h"
#include "meow/ui/widget/menu/DynamicMenu.h"

using namespace meow;

class WidgetCreator
{
public:
    WidgetCreator(GraphicsDriver &display);

    EmptyLayout *getEmptyLayout();
    Label *getDisplayDescription(uint16_t id, const char *text);
    Label *getMenuItem(uint16_t id, const char *text, uint8_t font_id = 2);
    NavBar *getNavbar(uint16_t id, const char *left, const char *middle, const char *right);
    DynamicMenu *getDynamicMenu(uint16_t id, IItemsLoader *loader);

private:
    GraphicsDriver &_display;
};
