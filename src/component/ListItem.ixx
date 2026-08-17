module;

#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <qdatetime.h>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QEnterEvent>
#include <QLabel>
#include <utility>

export module symusic.component.list_item;
import symusic.common;
import symusic.component.marqueel_label;
import symusic.entity.play_manager;
import symusic.entity.song_manager;
import symusic.entity.status_manager;
import symusic.utils.sync;

export class ListItem : public QWidget {
    Q_OBJECT

private:
    // UI 组件创建
    void setupUI();

    // 样式设置
    void setupDefaultStyle();
    void setupHoverStyle();
    void setupPlayingStyle();
    void setupLikeButtonStyle() const;

    // 业务逻辑
    void highLightCurrPlay();

public:
    explicit ListItem(PlayContext songCtx);

    [[nodiscard]] const SongPtr& getSong() const {
        return _songCtx.song;
    }

    void updateIconStatus() const;

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    signals:
        void likeStatusUpdated();

    void doubleClicked(const SongPtr& song);

private:
    PlayContext _songCtx;
    QTimer* _clickTimer;
    bool _pendingSingleClick{};
    bool _skipNextPress{};
    bool _isHovered{false};
    QPushButton* _likeButton;
};

#include "ListItem.moc"
