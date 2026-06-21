#pragma once

#include <QPushButton>
#include <QVBoxLayout>

#include "entity/Song.h"
#include "NavigationButton.h"
#include "entity/SongManager.h"

class PlaylistItem : public QWidget {
    Q_OBJECT

public:
    using SongPtr = SongManager::SongPtr;
    explicit PlaylistItem(const SongPtr& song, QString description, QWidget* parent = nullptr);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    SongPtr _song;
    QPushButton* _button;
    QString _description;
};
