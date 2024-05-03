#include <QApplication>
#include <QProcess>

#include "worker.h"
#include "main_window.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    Worker worker;

    MainWindow window(&worker);
    worker.start();

    window.resize(600, 300);
    window.show();

    return app.exec();
}
