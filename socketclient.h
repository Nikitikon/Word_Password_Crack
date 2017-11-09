#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>

#include "winsock2.h"

#include "cracker.h"

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr, SOCKET mySocket = 0, QString address = nullptr, Cracker *crack = nullptr);

signals:
    void finishedNet();
    void sendClientMassegeSignal(QString);


public slots:
    void erroeStopClient(QString errorMy);
    void stopClient();
    void processClient();

private:
    SOCKET clientSocket;
    QString clientAddress;
    Cracker *crack;


};

#endif // SOCKETCLIENT_H
