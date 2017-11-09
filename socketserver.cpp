#include "socketserver.h"

SocketServer::SocketServer(QObject *parent, Cracker *crac) : QObject(parent)
{
    crac = crac;
    stopListen = false;
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

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0)
    {
        erroeStopNet("Socket() error " + WSAGetLastError());
        return;
    }

    sendNetMassegeSignal("Create Socket");

    QApplication::processEvents();
    if(stopListen)
    {
        emit finishedNet();
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(6969);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSock, (sockaddr *)&addr, sizeof(addr)))
    {
        erroeStopNet("Error bind " + WSAGetLastError());
        return;
    }

    sendNetMassegeSignal("Bind");

    QApplication::processEvents();
    if(stopListen)
    {
        emit finishedNet();
        return;
    }

    serverListen();
}

void SocketServer::serverListen()
{
    if (listen(serverSock, 0x100))
    {
        erroeStopNet("Error listen " + WSAGetLastError());
        return ;
    }

    SOCKET clientSocket;
    SOCKADDR_IN clientAddr;

    int client_addr_size = sizeof(clientAddr);

    while ((clientSocket = accept(serverSock, (sockaddr *)&clientAddr, &client_addr_size)))
    {
        QString clientIp = QString::fromUtf8(inet_ntoa(clientAddr.sin_addr));

        sendNetMassegeSignal("Подключился " + clientIp);
    }
}

void SocketServer::erroeStopNet(QString errorMy)
{
    sendNetMassegeSignal(errorMy);
    stopNet();
}
void SocketServer::stopNet()
{
    stopListen = true;
    closesocket(serverSock);
    WSACleanup();
}
void SocketServer::processNet()
{
    initialization();
}

