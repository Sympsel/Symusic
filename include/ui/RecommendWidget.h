#pragma once

#include <QLabel>
#include <QMap>
#include <QWidget>

#include "PlaylistItem.h"

class RecommendWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecommendWidget(QWidget* parent = nullptr);
    ~RecommendWidget() override;

private:
    struct Alist {
        QList<PlaylistItem*> list;
        int begin{0};
        QPushButton* leftButton{};
        QPushButton* rightButton{};
        QWidget* widget{nullptr};
    };

    using Items = QList<PlaylistItem*>;

    // UI 组件创建
    void initPlaylist();
    [[nodiscard]] QWidget* createPlateWidget(const QString& name);
    [[nodiscard]] Items displayList(const QString& name) const;

    // 样式设置
    static void syncButtonStyle(const std::initializer_list<QPushButton*>& buttons,
                                int width,
                                int height);

    // 业务逻辑
    void updateRowSize();
    void updateButtonVisibility(const QString& name) const;
    void updateWidgetLayout(const QString& name);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QMap<QString, Alist> _contain;
    int _rowSize{5};
    QTimer* _resizeTimer;
};
