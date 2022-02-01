#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include "compteracine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void charger();
    void sauvegarder();
    void refreshTreeCompte();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
