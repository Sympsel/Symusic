#pragma once

#include <iostream>

#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"
#include "utils/Sync.hpp"

class ListItem : public QWidget {
    Q_OBJECT

private:
    void setupUI();

public:
    explicit ListItem(SongContext  songCtx);

    [[nodiscard]] const SongPtr& getSong() const {
        return _songCtx.song;
    }

    void updateIconStatus() const;

protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    void likeStatusUpdated();

    void doubleClicked(const SongPtr& song);

private:
    SongContext _songCtx;
    QTimer* _clickTimer;
    bool _pendingSingleClick{};
    bool _skipNextPress{};
    QPushButton* _likeButton;
};
