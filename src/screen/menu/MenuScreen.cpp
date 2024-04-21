#include "MenuScreen.h"

#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/const.h"

#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/navbar/NavBar.h"

MenuScreen::MenuScreen(GraphicsDriver &display) : IScreen(display)
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
    _menu->setItemHeight((_display.height() - NAVBAR_HEIGHT - 2) / 4); // Встановити висоту для кожного окремого елемента списку
                                                                       // В залежності від висоти меню і цього параметра, буде розраховано кількість видимих елементів
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

    Label *games_item = new Label(ID_GAMES, _display);
    _menu->addWidget(games_item);
    games_item->setText(STR_GAME_ITEM);
    games_item->setGravity(IWidget::GRAVITY_CENTER);
    games_item->setTextSize(2);
    games_item->setTextOffset(5);                    // Встановити відступ тексту зліва
    games_item->setFocusBorderColor(COLOR_LIME);     // Встановити колір рамки віджету, коли він у фокусі
    games_item->setFocusBackColor(COLOR_FOCUS_BACK); // Встановити колір фону віджету, коли він у фокусі
    games_item->setTextColor(COLOR_WHITE);
    games_item->setBackColor(COLOR_MENU_ITEM);
    games_item->setChangingBorder(true); // Увімкнути зміну кольору рамки, якщо на віджет встановлюється фокус
    games_item->setChangingBack(true);   // Увімкнути зміну кольору фону, якщо на віджет встановлюється фокус
    //
    Label *mp3_item = games_item->clone(ID_MP3);
    _menu->addWidget(mp3_item);
    mp3_item->setText(STR_MP3_ITEM);
    //
    Label *files_item = games_item->clone(ID_FILES);
    _menu->addWidget(files_item);
    files_item->setText(STR_FILES_ITEM);
    //
    Label *pref_item = games_item->clone(ID_PREFERENCES);
    _menu->addWidget(pref_item);
    pref_item->setText(STR_PREFERENCES_ITEM);
    //
    Label *firm_item = games_item->clone(ID_FIRMWARE);
    _menu->addWidget(firm_item);
    firm_item->setText(STR_FIRMWARE_ITEM);
    //
    Label *ex2_item = games_item->clone(ID_EXMPL2);
    _menu->addWidget(ex2_item);
    ex2_item->setText(STR_EXAMPL2_ITEM);
    //
    Label *ex3_item = games_item->clone(ID_EXMPL3);
    _menu->addWidget(ex3_item);
    ex3_item->setText(STR_EXAMPL3_ITEM);

    //------------------------------------------------------------------------------------------------------------------------ Встановлення кількості позицій скролбару

    _scrollbar->setMax(_menu->getSize());
}

void MenuScreen::loop()
{
}

void MenuScreen::update()
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
    else if (_input.isReleased(Input::PIN_SELECT))
    {
        _input.lock(Input::PIN_SELECT, 500);
        ok();
    }
    else if (_input.isReleased(Input::PIN_START))
    {
        _input.lock(Input::PIN_START, 500);
        _input.reset();
        openScreenByID(ID_SCREEN_HOME);
    }
}

void MenuScreen::up()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void MenuScreen::down()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void MenuScreen::ok()
{
    uint16_t id = _menu->getCurrentItemID(); // Отримати ідентифікатор виділеного елементу меню
    _input.reset();

    // Якщо список меню визначае екрани, тоді елементам меню в якості id можна задати id екранів та викликати одразу openScreenByID(id);
    // Але для прикладу буде так.
    if (id == ID_GAMES)
        openScreenByID(ID_SCREEN_GAMES);

}
