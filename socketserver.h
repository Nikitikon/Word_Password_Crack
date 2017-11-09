#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>

#include "winsock2.h"
#include "cracker.h"

class SocketServer : public QObject
{
    Q_OBJECT
public:
    explicit SocketServer(QObject *parent = nullptr, Cracker *crac = nullptr);

signals:
    void sendNetMassegeSignal(QString);
    void finishedNet();

public slots:
    void erroeStopNet(QString errorMy);
    void stopNet();
    void processNet();

private:
    SOCKADDR_IN addr;
    SOCKET serverSock;
    Cracker *crac;

    void initialization();
};

#endif // SOCKETSERVER_H
