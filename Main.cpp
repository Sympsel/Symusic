#include <QApplication>
#include <QFile>

import symusic.common;
import symusic.ui.main_window;

QString loadQss(const QString& path) {
    QFile file{path};
    if (!file.open(QFile::ReadOnly)) {
        return {};
    }
    return file.readAll();
}

int main(int argc, char* argv[]) {
    logConfig().filterLogLevel(1).showTimeOnly().withTID().withLineNumber().withColor();

    QApplication a(argc, argv);
    // a.setStyleSheet(loadQss(prefix::styleFiles + "vscode-light.qss"));
    a.setStyleSheet(loadQss(prefix::styleFiles + "vscode.qss"));
    MainWindow w(nullptr, true, false);
    w.show();

    return QApplication::exec();
}