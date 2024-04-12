#include "HomeScreen.h"
#include <pgmspace.h>
#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/const.h"
#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/navbar/NavBar.h"

HomeScreen::HomeScreen(GraphicsDriver &display) : IScreen{display}
{
    WidgetCreator creator{_display};
    //
    EmptyLayout *layout = creator.getEmptyLayout(1);
    setLayout(layout);

    // Опис екрану
    Label *display_descr = creator.getDisplayDescription(1, STR_HOME_HEADER);
    layout->addWidget(display_descr);

    // меню
    _menu = new FixedMenu(2, _display);
    _menu->setPos(0, DISPLAY_DESCRIPT_HEIGHT + 1);
    _menu->setBackColor(TFT_BLACK);
    _menu->setWidth(_display.width() - SCROLLBAR_WIDTH);
    _menu->setHeight(_display.height() - NAVBAR_HEIGHT - DISPLAY_DESCRIPT_HEIGHT - 2);
    _menu->setItemHeight(_menu->getHeight() / 5);
    layout->addWidget(_menu);

    // пункти меню
    Label *item_game = new Label(ID_SCREEN_GAME, _display);
    _menu->addWidget(item_game);
    item_game->setText(STR_GAME_ITEM);
    item_game->setGravity(IWidget::GRAVITY_CENTER);
    item_game->setTextSize(2);
    item_game->setTextOffset(5);
    item_game->setFocusBorderColor(COLOR_LIME);
    item_game->setBorder(true);
    item_game->setFocusBackColor(COLOR_FOCUS_BACK);
    item_game->setChangingBack(true);
    item_game->setChangingBorder(true);
    item_game->setTextColor(TFT_WHITE);
    item_game->setBackColor(TFT_BLACK);

    Label *item_test1 = item_game->clone(2);
    _menu->addWidget(item_test1);
    item_test1->setText("Test 1");

    Label *item_test2 = item_game->clone(3);
    _menu->addWidget(item_test2);
    item_test2->setText("Test 2");

    Label *item_test3 = item_game->clone(4);
    _menu->addWidget(item_test3);
    item_test3->setText("Test 3");

    // Скрол
    _scrollbar = new ScrollBar(3, _display);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_display.height() - NAVBAR_HEIGHT - DISPLAY_DESCRIPT_HEIGHT - 2);
    _scrollbar->setPos(_display.width() - SCROLLBAR_WIDTH, DISPLAY_DESCRIPT_HEIGHT + 1);
    _scrollbar->setMax(_menu->getSize());
    layout->addWidget(_scrollbar);

    // панель навігації
    NavBar *navbar = creator.getNavbar(4, "", STR_SELECT, "");
    layout->addWidget(navbar);
}

void HomeScreen::loop()
{
    // Тут можна виконувати псевдопаралельні задачі для поточного екрану
}

void HomeScreen::update()
{
    if (_input.isHolded(Input::PIN_UP))
    {
        _input.lock(Input::PIN_UP, 200);
        up();
    }
    else if (_input.isHolded(Input::PIN_DOWN))
    {
        _input.lock(Input::PIN_DOWN, 200);
        down();
    }
    else if (_input.isReleased(Input::PIN_START))
    {
        _input.reset();
        ok();
    }
}

void HomeScreen::up()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void HomeScreen::down()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void HomeScreen::ok()
{
    // uint16_t ID = _menu->getCurrentItemID();
    // openScreenByID(ID);
}
