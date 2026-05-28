#pragma once

#include "ListWidget.h"
#include "PlaylistBox.h"

class RecommendWidget : public QWidget {
private:
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons, int width, int height);

public:
    explicit RecommendWidget(QWidget* parent);

    static QWidget* createWidgetItem(const QString& name, const std::initializer_list<PlaylistBox*>& boxs);
};
