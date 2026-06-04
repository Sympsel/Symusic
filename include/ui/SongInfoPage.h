#pragma once

#include <QLabel>
#include <QPushButton>

#include "entity/Song.h"
#include "entity/Color.hpp"

class SongInfoPage : public QWidget {
    Q_OBJECT

public:
    explicit SongInfoPage(const Song& song, QWidget* parent = nullptr);

    void updateSong(const Song& song);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();
    void setBorder(bool enabled = false);
    void applyStyles();

    Song _song;

    QLabel* _coverLabel;
    QLabel* _nameLabel;
    QLabel* _artistLabel;
    QLabel* _albumLabel;
    QLabel* _durationLabel;
    QLabel* _tagsLabel;
    QPushButton* _likeButton;
    QPushButton* _playButton;
    QPushButton* _closeButton;
    QPoint _dragPos;
};
;
