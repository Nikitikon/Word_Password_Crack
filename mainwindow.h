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

#define PassLen 10
#define WorkStack 21
#define AlphabetLen 36


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
    QString netFileName;
    QAxObject* word;
    QAxObject* document;
    bool stopCalculating;
    char lastPass[PassLen];
    QStack<QString> needCalc;
    char alphabet[AlphabetLen];
    const quint64 maxVariant = 3656158440062976;



    void OpenWord();
    void initialization();
    bool checkFile();
    void erroeStop(QString errorMy);
    bool myCopyFile();
    void sendMassege(QString mass);

};

#endif // MAINWINDOW_H
