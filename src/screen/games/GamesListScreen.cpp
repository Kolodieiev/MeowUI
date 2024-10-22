#include "GamesListScreen.h"
#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/const.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/navbar/NavBar.h"

#include "ImageItem.h"
#include "./icons/snake_ico.h"
#include "./icons/sokoban_ico.h"

GamesListScreen::GamesListScreen(GraphicsDriver &display) : IScreen(display)
{
    //------------------------------------------------------------------------------------------------------------------------ Пам'ятка

    /*
     *  Якщо не знайшли опису якоїсь дії, або поведінки, шукайте його в попередніх екранах, що приводять до поточного.
     */

    //------------------------------------------------------------------------------------------------------------------------ Налаштування зовнішнього вигляду віджетів

    WidgetCreator creator{_display};
    //
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    _menu = new FixedMenu(ID_MENU, _display);
    layout->addWidget(_menu);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(_display.width() - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(_display.height() - NAVBAR_HEIGHT - 2);
    _menu->setItemHeight((_display.height() - NAVBAR_HEIGHT - 2) / 4);

    //
    _scrollbar = new ScrollBar(ID_SCROLLBAR, _display);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_display.height() - NAVBAR_HEIGHT);
    _scrollbar->setPos(_display.width() - SCROLLBAR_WIDTH, 0);
    //
    NavBar *navbar = creator.getNavbar(ID_NAVBAR, STR_SELECT, "", STR_BACK);
    layout->addWidget(navbar);

    //------------------------------------------------------------------------------------------------------------------------ Налаштування елементів меню

    ImageItem *snake_item = new ImageItem(ID_SCREEN_SNAKE, _display); // В якості ідентифікатора елементу присвоюється ідентифікатор екрану.
                                                                      // Завдяки цьому, не потрібно писати додаткову перевірку. Див. метод void ok();
    _menu->addWidget(snake_item);
    snake_item->setFocusBorderColor(COLOR_LIME);
    snake_item->setFocusBackColor(COLOR_FOCUS_BACK);
    snake_item->setBackColor(COLOR_MENU_ITEM);
    snake_item->setTextColor(COLOR_WHITE);
    snake_item->setChangingBorder(true); // Змінювати колір рамки коли віджет потрапляє у фокус
    snake_item->setChangingBack(true);   // Змінювати фоновий колір коли віджет потрапляє у фокус
    snake_item->setTickerInFocus(true);  // Вмикаємо прокрутку тексту, коли на елемент потрапляє фокус і ширини недостатньо, щоб вмістити увесь текст.

    ImageItem *sokoban_item = snake_item->clone(ID_SCREEN_SOKOBAN);
    _menu->addWidget(sokoban_item);

    //------------------------------------------------------------------------------------------------------------------------

    Label *snake_lbl = new Label(1, _display); // Налаштування віджету тексту
    snake_lbl->setText(STR_SNAKE_ITEM);
    snake_lbl->setTickerInFocus(true); // Вмикаємо біжучий рядок, коли на віджет потрапляє фокус і ширини віджета не достатньо, щоб відобразити його текст
    snake_lbl->setFocusBackColor(COLOR_FOCUS_BACK);
    snake_lbl->setBackColor(COLOR_MENU_ITEM);
    snake_lbl->setChangingBack(true);
    snake_lbl->setTextSize(2);
    snake_lbl->setChangingBack(true);
    //
    Image *snake_img = new Image(1, _display); // Налаштування іконки
    snake_img->init(32, 32);
    snake_img->setBackColor(TFT_DARKGREY);
    snake_img->setCornerRadius(5);
    snake_img->setTransparentColor(snake_img->TRANSPARENT_COLOR);
    snake_img->setSrc(ICO_SNAKE);
    //
    snake_item->setWidgets(snake_img, snake_lbl); // Встановити текст та іконку до елементу списку меню

    //------------------------------------------------------------------------------------------------------------------------
    Label *pack_lbl = snake_lbl->clone(1);
    pack_lbl->setText(STR_SOKOBAN_ITEM);
    //
    Image *pack_img = new Image(1, _display);
    pack_img->init(32, 32);
    pack_img->setBackColor(TFT_DARKGREY);
    pack_img->setCornerRadius(5);
    pack_img->setTransparentColor(snake_img->TRANSPARENT_COLOR);
    pack_img->setSrc(ICO_SOKOBAN);
    //
    sokoban_item->setWidgets(pack_img, pack_lbl);

    //------------------------------------------------------------------------------------------------------------------------
    _scrollbar->setMax(_menu->getSize());
}

void GamesListScreen::loop()
{
}

void GamesListScreen::update()
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
        _input.lock(Input::PIN_START, 500);
        openScreenByID(ID_SCREEN_MENU);
    }
    else if (_input.isReleased(Input::PIN_SELECT))
    {
        _input.lock(Input::PIN_SELECT, 500);
        ok();
    }
}

void GamesListScreen::up()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void GamesListScreen::down()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void GamesListScreen::ok()
{
    uint8_t screen_id = _menu->getCurrentItemID();

    if (screen_id == ID_SCREEN_SNAKE)
        return; // Гру ще не реалізовано

    openScreenByID((ScreenID)screen_id); // Ігровий екран буде відкриватися без додаткової перевірки.
}
