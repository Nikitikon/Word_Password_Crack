#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAxObject>
#include <QException>
#include <QDebug>
#include <QList>
#include <QStack>
#include <QFile>

#define PassLen 20
#define WorkStack 41
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



    void OpenWord();
    void initialization();
    bool chackFile();
};

#endif // MAINWINDOW_H
