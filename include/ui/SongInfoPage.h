#pragma once

#include <QLabel>
#include <QPushButton>

#include "entity/Song.h"
#include "entity/SongManager.h"

class SongInfoPage : public QWidget {
    Q_OBJECT

public:
    explicit SongInfoPage(const SongPtr& song, QWidget* parent = nullptr);

    void updateSong(const SongPtr& song);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

signals:
    void likeStatusChanged();

private:
    void setupUI();
    void setBorder(bool enabled = false);
    void applyStyles();

    SongPtr _song;

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
