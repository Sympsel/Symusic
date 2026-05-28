#pragma once

#include <QPushButton>

#include "NavigationButton.h"

class PlaylistBox : public QWidget {
public:
    explicit PlaylistBox(QString coverPath, QString description, QWidget* parent = nullptr);

private:
    QPushButton* _button;
    QString _coverPath;
    QString _description;
};
