#pragma once

// #include <qpixmap.h>

#include <QLabel>
#include <QPainter>
#include <QPainterPath>

class QFrame;
class QLabel;

class Create {
public:
    static QPixmap circularPixmap(const QString& picture, const int size) {
        const QPixmap source(picture);
        QPixmap target(size, size);
        // 透明背景
        target.fill(Qt::transparent);
        QPainter painter(&target);
        // 抗锯齿
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QPainterPath path;
        path.addEllipse(0, 0, size, size);
        painter.setClipPath(path);

        const QPixmap scaled = source.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        const int x = (scaled.width() - size) / 2;
        const int y = (scaled.height() - size) / 2;
        painter.drawPixmap(0, 0, scaled, x, y, size, size);
        return target;
    }

    static QLabel* squarePixmap(QWidget* parent, const QString& picture, const int size) {
        const auto pictureLabel = new QLabel(parent);
        QPixmap source(picture);
        source = source.scaled(size, size);
        pictureLabel->setPixmap(source);
        return pictureLabel;
    }


    static QFrame* line(const QFrame::Shape shape) {
        const auto line = new QFrame();
        line->setFrameShape(shape);
        line->setFrameShadow(QFrame::Sunken); // 凹陷阴影效果
        line->setLineWidth(1);
        return line;
    }
};