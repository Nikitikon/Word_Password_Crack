#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QFile>
#include <QQueue>

#define TimeOut 30

#include "winsock2.h"

#include "cracker.h"

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr, SOCKET mySocket = 0, QString address = nullptr, Cracker *crack = nullptr);

signals:
    void finishedClient();
    void sendClientMassegeSignal(QString);
    void criticalErrorSignal();


public slots:
    void erroeStopClient(QString errorMy);
    void stopClient();
    void processClient();

private:
    SOCKET clientSocket;
    QString clientAddress;
    Cracker *crack;
    bool stopWorkClient;
    QString id;

    QString lastMass;
    QQueue<QString> commandQueue;

    bool sendAll(QString mass);
    bool sendFile();
    bool sendAllByte(QByteArray block, qint64 len);
    bool getMassege();
    void setSpeed(double speed);
    void removeSpeed();

};

#endif // SOCKETCLIENT_H
