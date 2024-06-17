#include "HomeScreen.h"
//
#include "../resources/color.h"
#include "../resources/string.h"
#include "../resources/ico/battery.h"
//
#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/navbar/NavBar.h"
//
#include "meow/util/bmp/BmpUtil.h"

HomeScreen::HomeScreen(GraphicsDriver &display) : IScreen{display}
{
    WidgetCreator creator{_display}; // Для створення типових віджетів. Використовуй на власний розсуд.

    //------------------------------------------------------------------------------------------------------------------------ Пам'ятка

    /*
     Важливо пам'ятати:
     Віджети малюються по черзі від першого. Тобто наступний доданий віджет буде вище по Z сортуванню за попередній.
     Ідентифікатор кожного віджета повинен бути унікальним для свого шаблону. Рекомендується використовувати enum для цих цілей.
     Порядок в enum повинен починатися зі значення > 0.
    */

    //------------------------------------------------------------------------------------------------------------------------ Створення макету

    EmptyLayout *layout = creator.getEmptyLayout(); // Створити макет
    setLayout(layout);                              // Встановити макет як основний для поточного екрану

    //------------------------------------------------------------------------------------------------------------------------ Завантаження зображення із SD

    BmpUtil util;                                      // Об'єкт, що завантажує bmp із SD
    BmpData bmp = util.loadBmp("/wall/wallpaper.bmp"); // Структура, куди будуть завантажені дані про зображення

    if (bmp.is_loaded)
    {
        _wallpaper_ptr = bmp.data_ptr; // Закешувати вказівник на зображення. Воно буде видалене в даному випадку в деструкторі.
                                       // Якщо util.loadBmp() == false, видаляти нічого не потрібно. Повернутий вказівник буде вказувати на зображення в FLASH

        Image *wallpp_img = new Image(ID_WALLPAPER, _display); // Створення віджету зображення, яке буде фоновим для екрану
        layout->addWidget(wallpp_img);                         // Додати віджет до макету. Рекомендується одразу додавати віджет після його створення до макету, аби потім не забути
        wallpp_img->init(bmp.width, bmp.height);               // Ініціалізувати зображення відповідно до розмірів завантаженного bmp. Метод недоступний, якщо вимкнута подвійна буферизація
        wallpp_img->setSrc(_wallpaper_ptr);                    // Встановити вказівник на зображення в PSRAM/FLASH. Ресурс не буде звільнено разом з віджетом. Програміст повинен сам контролювати очищення зображення.
                                                               // Перед викликом setSrc() при подвійній буферизації завжди потрібно викликати метод init.

        wallpp_img->setPos(
            (_display.width() - wallpp_img->getWidth()) * 0.5,
            (_display.height() - wallpp_img->getHeight()) * 0.5); // Встановити віджет по центру екрана
    }

    //------------------------------------------------------------------------------------------------------------------------ Створення текстової мітки із зображенням

    Label *bat_lvl_lbl = new Label(ID_BAT_LVL, _display); // Створити віджет текстової мітки для індикатора заряду акумулятора
    layout->addWidget(bat_lvl_lbl);
    bat_lvl_lbl->setText("100");
    bat_lvl_lbl->setWidth(56);                      // Ширина віджету
    bat_lvl_lbl->setHeight(35);                     // Висота віджету
    bat_lvl_lbl->setAlign(Label::ALIGN_CENTER);     // Встановити положення тексту в центр віджета по горизонталі
    bat_lvl_lbl->setGravity(Label::GRAVITY_CENTER); // Встановити положення тексту в центр віджета по вертикалі
    bat_lvl_lbl->setPos(_display.width() - 60, 3);  // Розмістити віджет у верхньому правому куті екрану

    _bat_ico = new Image(1, _display); // Створити зображення, яке буде використовуватися в якості фону для мітки індикатора зарядку акума.
                                       // Зображення необхідно кешувати і проконтрювати видалення. Тому що мітка не видаляє самостійно фонове зображення.
                                       // Завдяки цьому одне і те ж зображення може бути перевикористано багато разів у різних віджетах.

    bat_lvl_lbl->setBackImg(_bat_ico); // Встановити зображення як фонове для текстової мітку
    _bat_ico->init(56, 35);
    _bat_ico->setSrc(ICO_BATTERY);                              // Встановити вказівник на зображення у FLASH
    _bat_ico->setTransparentColor(_bat_ico->TRANSPARENT_COLOR); // Задати колір, який буде ігноруватися при малюванні зображення, створюючи ефект прозорості

    //------------------------------------------------------------------------------------------------------------------------ Створення звичайної текстової мітки

    Label *time_lbl = new Label(ID_TIME_LBL, _display);
    layout->addWidget(time_lbl);
    time_lbl->setText("00:00");
    time_lbl->setAlign(Label::ALIGN_CENTER);
    time_lbl->setGravity(Label::GRAVITY_CENTER);
    time_lbl->setFontID(2);     // Встановити шрифт. По замовченю встановлено шрифт з id 2. Може бути 2 або 4. Шрифт 2 має висоту 15пкс. 4 - 9пкс.
    time_lbl->setTextSize(2);   // Розмір розраховується по формулі default_font_size * k. Де k - параметр, який передається у цей метод.
    time_lbl->initWidthToFit(); // Ініціалізувати ширину віджета, шириною тексту, що додано в нього. Текст повинен бути встановлений до виклику методу.
                                // Не викликайте updateWidthToFit для ініціалізації ширини віджета. Він не матиме сили під час першого малювання.

    time_lbl->setPos(5, 0);

    //------------------------------------------------------------------------------------------------------------------------ Клонування віджетів

    Label *date_lbl = new Label(ID_DATE_LBL, _display);
    layout->addWidget(date_lbl);
    date_lbl->setText("18.04.2024");
    date_lbl->setAlign(Label::ALIGN_CENTER);
    date_lbl->setGravity(Label::GRAVITY_CENTER);
    date_lbl->setPos(5, time_lbl->getHeight() + 3);
    date_lbl->initWidthToFit();

    Label *day_lbl = date_lbl->clone(ID_DAY_LBL); // Створити віджет, що буде глибокою копією об'єкта, з якого він клонується.
    layout->addWidget(day_lbl);
    day_lbl->setText("Четвер");
    date_lbl->setPos(5, date_lbl->getYPos() + date_lbl->getHeight() + 3);
    day_lbl->initWidthToFit();

    //------------------------------------------------------------------------------------------------------------------------ Створення простої ілюзії прозорості віджетів

    if (!_wallpaper_ptr) // Якщо фонове зображення не завантажено із SD
    {
        _bat_ico->setBackColor(COLOR_MAIN_BACK); // Встановлюємо той колір, що і у основного макету. Фоновий колір зображення буде вищим, аніж у мітки. Тому що він малюється пізніше.
        time_lbl->setBackColor(COLOR_MAIN_BACK);
        date_lbl->setBackColor(COLOR_MAIN_BACK);
        day_lbl->setBackColor(COLOR_MAIN_BACK);
    }
    else
    {
        uint16_t back_bat_color = *(_wallpaper_ptr + bat_lvl_lbl->getYPos() * bmp.width + bat_lvl_lbl->getXPos()); // Встановити фоновий колір, як у фонового зображення заантаженого із SD.
                                                                                                                   // Припускається, що фонове зображення має достатні розміри.
                                                                                                                   // Прийом працюватиме гарно, якщо фонове зображення не дуже барвисте.

        _bat_ico->setBackColor(back_bat_color);

        uint16_t back_lbl_color = *(_wallpaper_ptr + time_lbl->getYPos() * bmp.width + time_lbl->getXPos());

        time_lbl->setBackColor(back_lbl_color);
        date_lbl->setBackColor(back_lbl_color);
        day_lbl->setBackColor(back_lbl_color);
    }

    //------------------------------------------------------------------------------------------------------------------------ Додавання панелі навігації

    NavBar *navbar = creator.getNavbar(ID_NAVBAR, STR_MENU, "", ""); // Колір, розмір тексту тощо, можна визначити, якщо створювати об'єкт самостійно
                                                                     // Або відредагувавши створення цього віджета у WidgetCreator
    layout->addWidget(navbar);
}

HomeScreen::~HomeScreen()
{
    //------------------------------------------------------------------------------------------------------------------------ Вивільнення ресурсів

    delete _bat_ico;
    free((void *)_wallpaper_ptr);
}

void HomeScreen::loop()
{
    // Тут можна виконувати псевдопаралельні задачі для поточного екрану
}

void HomeScreen::update()
{
    //------------------------------------------------------------------------------------------------------------------------ Обробка вводу

    if (_input.isReleased(Input::PIN_SELECT))
    {
        _input.lock(Input::PIN_SELECT, 500); // Заблокувати натискання для цієї клаваші на 500мс. Може бути використано як для подавлення брязчяння на тактових кнопках,
                                             // так і для керування частотою спрацювань для кожної клавіші окремо.
        openScreenByID(ID_SCREEN_MENU);
    }

    //------------------------------------------------------------------------------------------------------------------------ Оновлення віджетів
    if (millis() - _upd_timer > 1000)
    {
        _upd_timer = millis();

        ++_m_counter;

        if (_m_counter > 59)
        {
            _m_counter = 0;
            ++_h_counter;

            if (_h_counter > 23)
                _h_counter = 0;
        }

        String temp_str = ""; // Рядок, в який буде сформовано поточний час

        if (_h_counter < 10)
            temp_str = "0";

        temp_str += _h_counter;
        temp_str += ":";

        if (_m_counter < 10)
            temp_str += "0";

        temp_str += _m_counter;

        Label *lbl = (Label *)getLayout()->findWidgetByID(ID_TIME_LBL); // Вказівник на віджет рекомендується закешувати, якщо віджет часто оновлюється.
                                                                        // Закешовані вказівники на віджети видаляти не потрібно!! Контейнер сам видалить свої віджети.
                                                                        // Також його можна діставати з контейнера перед оновленням, що займатиме певний час.

        lbl->setText(temp_str);
        // lbl->updateWidthToFit(); // Можна підігнати ширину віджету під текст, якщо необхідно також контролювати коректну перемальовку фону.
    }
}
