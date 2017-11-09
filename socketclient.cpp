#include "socketclient.h"

SocketClient::SocketClient(QObject *parent, SOCKET mySocket, QString address, Cracker *crack) : QObject(parent)
{
    this->clientSocket = mySocket;
    this->clientAddress = address;
    this->crack = crack;
}

void SocketClient::erroeStopClient(QString errorMy)
{

}

void SocketClient::stopClient()
{

}

void SocketClient::processClient()
{
    send(clientSocket, "OK", strlen("OK"), 0);
}
