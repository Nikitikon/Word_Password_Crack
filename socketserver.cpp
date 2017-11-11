#include "socketserver.h"

SocketServer::SocketServer(QObject *parent, Cracker *crac) : QObject(parent)
{
    this->crac = crac;
    stopListen = false;
}

void SocketServer::initialization()
{
    stopListen = false;

    char wsaBuff[1024];
    if (WSAStartup(0x202, (WSADATA *)&wsaBuff))
    {
        erroeStopNet("WSAStart error " + WSAGetLastError());
        return;
    }

    sendNetMassegeSignal("WSA Start");

    serverSock = socket(AF_INET, SOCK_STREAM, 0 );
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
    QApplication::processEvents();

    struct timeval timeout;
    struct fd_set fds;
    timeout.tv_sec = 0;
    timeout.tv_usec = 250000;

    while (!stopListen)
    {
        QApplication::processEvents();

        FD_ZERO(&fds);
        FD_SET(serverSock, &fds);
        int slct = select(0, &fds, 0, 0, &timeout);

        if (slct == -1) continue;

        if (slct != 0)
        {
            if((clientSocket = accept(serverSock, (sockaddr *)&clientAddr, &client_addr_size)))
            {
                QString clientIp = QString::fromUtf8(inet_ntoa(clientAddr.sin_addr));
                if(!stopListen)
                    sendNetMassegeSignal("Подключился " + clientIp);

                SocketClient *client = new SocketClient(nullptr, clientSocket, clientIp, crac); //@!!!!!!!!!!!!

                QThread *thread = new QThread;

                client->moveToThread(thread);

                connect(thread, SIGNAL(started()), client, SLOT(processClient()));

                connect(client, SIGNAL(finishedClient()), thread, SLOT(quit()));

                connect(this, SIGNAL(stopAllClient()), client, SLOT(stopClient()));

                connect(client, SIGNAL(finishedClient()), client, SLOT(deleteLater()));

                connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

                connect(client, SIGNAL(sendClientMassegeSignal(QString)), this, SLOT(clientMassege(QString)));

                connect(client, SIGNAL(criticalErrorSignal()), crac, SLOT(stop()));

                thread->start();
            }
        }
    }
}

void SocketServer::erroeStopNet(QString errorMy)
{
    emit stopAllClient();
    sendNetMassegeSignal(errorMy);
    closesocket(serverSock);
    WSACleanup();
}
void SocketServer::stopNet()
{
    stopListen = true;
    erroeStopNet("Сервер остановлен");
}
void SocketServer::processNet()
{
    initialization();
    serverListen();
}

void SocketServer::clientMassege(QString mass)
{
    emit sendNetMassegeSignal(mass);
}

