#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAxObject>
#include <QException>
#include <QDebug>
#include <QList>
#include <QStack>
#include <QFile>
#include <QFileDialog>
#include <QtGlobal>
#include <QMutex>
#include <QtMath>
#include <QTimer>
#include <QThread>

#include "outputthread.h"
#include "cracker.h"
#include "socketserver.h"

#define PassLen 10
#define WorkStack 21
#define AlphabetLen 15


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Open_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;


    Cracker *crac;
    SocketServer *serv;


    void OpenWord();
    void initialization();

signals:
    void startWork(QString);
    void stopAll();

private slots:
    void sendMassege(QString mass);
    void sendNetMassege(QString mass);

};

#endif // MAINWINDOW_H
