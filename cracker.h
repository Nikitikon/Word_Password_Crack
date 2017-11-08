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

    bool getPasswordInterval(int interval[]);


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
    int lastPass;
    QStack<QString> needCalc;
    QQueue<QString> calcOnClient;
    char alphabet[AlphabetLen];
    quint64 maxVariant;
    QList<QString> answer;
    QMutex mutexForInterval;
    QMutex mutexForAnswerList;
    QMutex mutexForNeedList;
    int bitMasck;
    int passwordStep;

    QLineEdit *fileNameBuff;
    QTextBrowser *outBrowser;

    void initialization();
    bool checkFile();
    bool myCopyFile();


};

#endif // CRACKER_H
