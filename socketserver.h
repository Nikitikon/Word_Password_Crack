#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>

#include "winsock2.h"
#include "cracker.h"
#include "socketclient.h"

class SocketServer : public QObject
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = nullptr, Cracker *crac = nullptr);

signals:
    void sendNetMassegeSignal(QString);
    void finishedNet();
    void stopAllClient();

public slots:
    void erroeStopNet(QString errorMy);
    void stopNet();
    void processNet();
    void clientMassege(QString mass);

private:
    SOCKADDR_IN addr;
    SOCKET serverSock;
    Cracker *crac;
    bool stopListen;

    void initialization();
    void serverListen();
};

#endif // SOCKETSERVER_H
