#include <QApplication>
#include <QFile>
#include <QLoggingCategory>
#include <random>

#include "MainWindow.h"
#include "Log.hpp"

QString loadQss(const QString& path) {
    QFile file{path};
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Failed to load style style";
        return {};
    }
    return file.readAll();
}

int main(int argc, char* argv[]) {
    // 启用 Windows 控制台 ANSI 颜色支持，用于输出带颜色的调试信息
    sym::InitAnsiSupport();
    QApplication a(argc, argv);
    // a.setStyleSheet(loadQss(":/style/vscode-light.qss"));
    a.setStyleSheet(loadQss(":/style/vscode.qss"));
    MainWindow w(nullptr, true, true);
    w.show();

    return QApplication::exec();
}
