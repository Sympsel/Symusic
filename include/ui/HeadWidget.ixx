module;

#include <QPushButton>
#include <QWidget>

#include "entity/Common.hpp"

export module symusic.ui.head_widget;

export class HeadWidget final : public QWidget {
    Q_OBJECT

public:
    explicit HeadWidget(QWidget* parent = nullptr);
    QWidget* createFunctionWidget(QWidget* parent);
    static void syncButtonBackground(const std::initializer_list<QPushButton*>& buttons);
    signals:
        // 向父窗口发送请求
        void closeRequested();
    void minimizeRequested();
    void maximizeRequested();
};

