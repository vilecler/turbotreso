#include <QApplication>
#include <QtWidgets>
#include "mainwindow.h"
#include "compteracine.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow fenetre_princ;
    fenetre_princ.show();

    return app.exec();
}
