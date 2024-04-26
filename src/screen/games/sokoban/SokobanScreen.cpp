#include "SokobanScreen.h"
//
#include "../../WidgetCreator.h"
#include "./SceneID.h"
// Тут необхідно підключити всі сцени, які будуть доступні в грі
#include "scene/Level0Scene.h"
#include "scene/Level1Scene.h"
#include "scene/Level2Scene.h"

SokobanScreen::SokobanScreen(GraphicsDriver &display) : IScreen{display}
{
    WidgetCreator creator{_display};
    // Створити порожній макет екрану
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    // Створити першу ігрову сцену
    _scene = new Level0Scene(display, _input, _stored_objs, false);
}

void SokobanScreen::loop()
{
}

void SokobanScreen::update()
{
    if (!_scene->isFinished()) // Перевірити чи гру не завершено
    {
        if (!_scene->isReleased()) // Якщо не завершено, перевірити чи не потрібно змінити сцену
            _scene->update();      // Якщо не потрібно перемикати на іншу, оновити поточну
        else
        {
            uint8_t next_scene_id = _scene->getNextSceneID(); // Інакше, якщо потрібно змінити сцену, отримуємо id наступної сцени
            delete _scene;                                    // Видаляємо поточну
            if (next_scene_id == ID_SCENE_LVL0)               // Створюємо нову в залежності від ідентифікатора
                _scene = new Level0Scene(_display, _input, _stored_objs, true);
            else if (next_scene_id == ID_SCENE_LVL1)
                _scene = new Level1Scene(_display, _input, _stored_objs, true);
            else if (next_scene_id == ID_SCENE_LVL2)
                _scene = new Level2Scene(_display, _input, _stored_objs, true);
        }
    }
    else
    {
        delete _scene;                        // Якщо гру завршено, видалити поточну сцену
        openScreenByID(ID_SCREEN_GAMES_LIST); // Відкрити екран із заданим id
    }
}
