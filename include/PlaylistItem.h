#pragma once

#include <QPushButton>

#include "NavigationButton.h"

class PlaylistItem : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistItem(QString coverPath, QString description, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    QPushButton* _button;
    QString _coverPath;
    QString _description;
};
