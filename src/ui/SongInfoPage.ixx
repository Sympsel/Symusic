module;

#include <QLabel>
#include <QPushButton>

export module symusic.ui.song_info_page;
import symusic.entity.play_manager;
import symusic.entity.song_manager;

export class SongInfoPage : public QWidget {
    Q_OBJECT

private:
    void setupUI();
    void setupButtonStyle();
    void setupButtonConnect();

public:
    explicit SongInfoPage(const PlayContext& songCtx, QWidget* parent = nullptr);

    void updateSong(const SongPtr& song);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    signals:
        void likeStatusChanged();

private:
    PlayContext _songCtx;
    bool _originLiked;
    bool _willLike;

    QLabel* _coverLabel;
    QLabel* _nameLabel;
    QLabel* _artistLabel;
    QLabel* _albumLabel;
    QLabel* _durationLabel;
    QLabel* _playCountLabel;
    QLabel* _tagsLabel;
    QLabel* _listsLabel;
    QPushButton* _likeButton;
    QPushButton* _playButton;
    QPushButton* _closeButton;
    QPoint _dragPos;
};
;
