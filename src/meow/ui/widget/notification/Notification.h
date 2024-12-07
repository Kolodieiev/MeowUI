#pragma once
#include <Arduino.h>
#include "../IWidget.h"
#include "../text/Label.h"

namespace meow
{

    class Notification : public IWidget
    {

    public:
        Notification(uint16_t widget_ID, GraphicsDriver &display);
        virtual ~Notification();
        virtual void onDraw() override;
        // STUB! Не викликай!
        Notification *clone(uint16_t id) const override
        {
            log_e("Клонування неможливе");
            esp_restart();
            return nullptr;
        }

        void setLabels(Label *title, Label *msg, Label *left_btn, Label *right_btn);

        inline void show() { _is_hidden = false; }
        inline void hide() { _is_hidden = true; }
        inline bool isHidden() const { return _is_hidden; }

    private:
        using IWidget::getFocusBackColor;
        using IWidget::getFocusBorderColor;
        using IWidget::setFocus;
        using IWidget::setFocusBackColor;
        using IWidget::setFocusBorderColor;
        using IWidget::setHeight;
        using IWidget::setVisibility;

        Label *_title_lbl{nullptr};
        Label *_msg_lbl{nullptr};
        Label *_left_lbl{nullptr};
        Label *_right_lbl{nullptr};

        bool _is_hidden{true};
    };

}