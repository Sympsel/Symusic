#pragma once

#include <QPushButton>
#include <QVBoxLayout>

#include "NavigationButton.h"
#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"

class PlaylistItem : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistItem(const SongContext& songCtx, QString description, QWidget* parent = nullptr);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    SongContext _songCtx;
    QPushButton* _button;
    QString _description;
};
