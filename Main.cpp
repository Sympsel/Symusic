#include <QApplication>
#include <QFile>
#include <QLoggingCategory>

#include "ui/MainWindow.h"
#include "include/utils/Log.hpp"

QString loadQss(const QString& path) {
    QFile file{path};
    if (!file.open(QFile::ReadOnly)) {
        return {};
    }
    return file.readAll();
}

int main(int argc, char* argv[]) {
    // 启用 Windows 控制台 ANSI 颜色支持，用于输出带颜色的调试信息
    // sym::InitAnsiSupport();
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/resource/images/Sympsel.png"));
    a.setStyleSheet(loadQss(prefix::styleFiles + "vscode.qss"));
    MainWindow w(nullptr, true, false);
    w.show();

    return QApplication::exec();
}
