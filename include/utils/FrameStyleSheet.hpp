#pragma once

#include <QWidget>

#include "entity/Color.hpp"
#include "Log.hpp"

class FrameStyleSheet {
public:
    static void setBorder(QWidget* widget, const bool enabled = false) {
        const Color& color = ColorTheme::getInstance().getColor();
        if (enabled) {
            if (widget) {
                LOG_DEBUG() << "启用调试边框";
                widget->setStyleSheet(QString(
                    "background-color: rgb(%1);"
                    "border: 2px solid rgb(%2);"
                ).arg(color.background, color.border));
            }
        } else {
            if (widget) {
                LOG_DEBUG() << "启用常规边框";
                widget->setObjectName("MainCentralWidget");
                widget->setStyleSheet(QString(
                    "#MainCentralWidget {"
                    "   background-color: rgb(%1);"
                    "   border: 2px solid rgb(%2);"
                    "}"
                ).arg(color.background, color.border));
            }
        }
    }
};
