#ifndef CRACKER_H
#define CRACKER_H

#include <QObject>
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
#include <QLineEdit>
#include <QTextBrowser>
#include <QQueue>
#include <QApplication>


#include "exceptreceiver.h"


#define PassLen 10
#define WorkStack 21
#define AlphabetLen 15

class Cracker : public QObject
{
    Q_OBJECT
public:
    explicit Cracker(QObject *parent = nullptr);
    ~Cracker();

    bool getPasswordInterval(quint64 interval[]);
    bool setCalcOnClient(quint64 interval[]);
    bool setNeedCalc(quint64 interval[]);
    bool setAnswer(QString assumedKey);
    bool getCalcOnClient(quint64 interval[]);
    bool getNeedCalc(quint64 interval[]);
    QString getTryAnswer();


    QString fileName;



signals:
    void sendMassegeSignal(QString);
    void finished();

public slots:
    void erroeStop(QString errorMy);
    void stop();
    void process();



private:
    QString netFileName;
    QAxObject* word;
    QAxObject* document;
    bool stopCalculating;
    quint64 lastPass;
    QStack<QString> needCalc;
    QQueue<QString> calcOnClient;
    char alphabet[AlphabetLen];
    quint64 maxVariant;
    QQueue<QString> answer;
    QMutex mutexForInterval;
    QMutex mutexForAnswerList;
    QMutex mutexForNeedList;
    QMutex mutexForClientCalcList;
    int bitMasck;
    int passwordStep;

    QLineEdit *fileNameBuff;
    QTextBrowser *outBrowser;

    void initialization();
    bool checkFile();
    bool myCopyFile();
    QString converInterval(quint64 interval[]);
    bool converString(QString str , quint64 interval[]);
    bool fromClientCalcToNeedCalc();
    QString createPass(int step);
    bool crackPassword();

};

#endif // CRACKER_H
