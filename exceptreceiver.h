#ifndef EXCEPTRECEIVER_H
#define EXCEPTRECEIVER_H
#include <QObject>
#include <QDebug>

class ExceptReceiver : public QObject
{
    Q_OBJECT
public:
    ExceptReceiver():QObject(){}

public slots:
    void debugError(int errorCode, QString source, QString description, QString help)
    {
        qDebug()<<errorCode<<source<<description<<help;
    }
};
#endif // EXCEPTRECEIVER_H
