module;

#include <iostream>

#include "entity/PlayManager.hpp"
#include "entity/SongManager.h"
#include "utils/Sync.hpp"

export module symusic.ui.list_item;

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
    explicit ListItem(SongContext  songCtx);

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
    SongContext _songCtx;
    QTimer* _clickTimer;
    bool _pendingSingleClick{};
    bool _skipNextPress{};
    bool _isHovered{false};
    QPushButton* _likeButton;
};
