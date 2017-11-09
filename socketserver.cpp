#include "socketserver.h"

SocketServer::SocketServer(QObject *parent, Cracker *crac) : QObject(parent)
{
    crac = crac;
}

void SocketServer::initialization()
{
    char wsaBuff[1024];
    if (WSAStartup(0x202, (WSADATA *)&wsaBuff))
    {
        erroeStopNet("WSAStart error " + WSAGetLastError());
        return;
    }

    sendNetMassegeSignal("WSA Start");
}

void SocketServer::erroeStopNet(QString errorMy)
{
    sendNetMassegeSignal(errorMy);
}
void SocketServer::stopNet(){}
void SocketServer::processNet()
{
    initialization();
}

