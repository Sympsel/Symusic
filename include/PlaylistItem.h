#pragma once

#include <QPushButton>
#include <QVBoxLayout>

#include "Song.h"
#include "NavigationButton.h"

class PlaylistItem : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistItem(Song song, QString description, QWidget* parent = nullptr);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Song _song;
    QPushButton* _button;
    QString _description;
};
