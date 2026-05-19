#include "mainwindow.h"
#include "login.h"

#include <QApplication>
#include <QSettings>
#include <QString>

#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("NagaevM", "headmanApp");

    config conf;

    MainWindow w;
    login l;

    if (settings.contains(QString("auth_token")) && settings.contains(QString("name"))) {
        w.show();
    } else {
        l.show();
    }

    return QCoreApplication::exec();
}
