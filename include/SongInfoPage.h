#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "Song.h"
#include "Color.hpp"

class SongInfoPage : public QWidget {
    Q_OBJECT

public:
    explicit SongInfoPage(const Song& song, QWidget* parent = nullptr);

    void updateSong(const Song& song);

private:
    void setupUI();
    void applyStyles();
    void keyPressEvent(QKeyEvent* event) override;

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
};
;