#include "socketclient.h"

SocketClient::SocketClient(QObject *parent, SOCKET mySocket, QString address, Cracker *crack) : QObject(parent)
{
    this->clientSocket = mySocket;
    this->clientAddress = address;
    this->crack = crack;

    stopWorkClient = false;
    lastMass.clear();
    commandQueue.clear();

    id = QString::number(mySocket);
}

void SocketClient::erroeStopClient(QString errorMy)
{
    removeSpeed();
    sendClientMassegeSignal(errorMy);
    closesocket(clientSocket);
}

void SocketClient::stopClient()
{
    removeSpeed();
    stopWorkClient = true;
}

void SocketClient::processClient()
{
    bool nopeIterval = false;
    quint64 interval[2] = {0,0};

    while (!stopWorkClient)
    {
        QTime time;
        time.start();
        if(!getMassege())
        {

            if(nopeIterval)
                crack->setNeedCalc(interval);
            erroeStopClient("Отключен " + clientAddress);
            stopWorkClient = true;
            return;
        }

        while (!commandQueue.isEmpty() && !stopWorkClient)
        {
            QString command = commandQueue.dequeue();
            if(command == "DOC")
            {
                removeSpeed();
                if(!sendFile())
                {
                    if(nopeIterval)
                        crack->setNeedCalc(interval);
                    erroeStopClient("Отключен " + clientAddress);
                    stopWorkClient = true;
                    return;
                }

                continue;
            }

            if(command == "LEND")
            {
                removeSpeed();
                QFile fileNet("netDoc.docx");

                if (!fileNet.exists())
                {
                    if(!QFile::copy(crack->fileName, "netDoc.docx"))
                    {
                        if(nopeIterval)
                            crack->setNeedCalc(interval);
                        emit criticalErrorSignal();
                    }
                }
                QString len = QString::number(fileNet.size());
                if(!sendAll(len + "\r\n"))
                {
                    if(nopeIterval)
                        crack->setNeedCalc(interval);
                    erroeStopClient("Отключен " + clientAddress);
                    stopWorkClient = true;
                    return;
                }

                continue;
            }

            if(command == "MASK")
            {
                removeSpeed();
                QString mask = QString::number(crack->getMask());
                if(!sendAll(mask + "\r\n"))
                {
                    if(nopeIterval)
                        crack->setNeedCalc(interval);
                    erroeStopClient("Отключен " + clientAddress);
                    stopWorkClient = true;
                    return;
                }

                continue;
            }

            if(command == "INT")
            {
                if(nopeIterval)
                {
                    if(!sendAll("No answer\r\n"))
                    {
                        if(nopeIterval)
                            crack->setNeedCalc(interval);
                        erroeStopClient("Отключен " + clientAddress);
                        stopWorkClient = true;
                        return;
                    }

                    continue;
                }

                if(!crack->getNeedCalc(interval))
                {
                    if(!crack->getPasswordInterval(interval))
                    {
                        if(!sendAll("No interval\r\n"))
                        {
                            if(nopeIterval)
                                crack->setNeedCalc(interval);
                            erroeStopClient("Отключен " + clientAddress);
                            stopWorkClient = true;
                            return;
                        }

                        continue;
                    }
                }

                nopeIterval = true;

                time.restart();

                QString first = QString::number(interval[0]);
                QString second = QString::number(interval[1]);

                if(!sendAll(first + "\r\n" + second + "\r\n"))
                {
                    if(nopeIterval)
                        crack->setNeedCalc(interval);
                    erroeStopClient("Отключен " + clientAddress);
                    stopWorkClient = true;
                    return;
                }

                continue;
            }

            if(command == "NOPE")
            {
                if(!nopeIterval)
                {
                    if(!sendAll("Don't set interval\r\n"))
                    {
                        if(nopeIterval)
                            crack->setNeedCalc(interval);
                        erroeStopClient("Отключен " + clientAddress);
                        stopWorkClient = true;
                        return;
                    }

                    continue;
                }

                int workTime = time.elapsed();
                setSpeed((double)crack->getStep() / workTime);

                crack->setCalcOnClient(interval);

                nopeIterval = false;

                continue;
            }

            if(command.contains("ANS"))
            {

                QString temp = command.mid(0, 3);
                if(temp == "ANS")
                {
                    if(!nopeIterval)
                    {
                        if(!sendAll("Don't set interval\r\n"))
                        {
                            if(nopeIterval)
                                crack->setNeedCalc(interval);
                            erroeStopClient("Отключен " + clientAddress);
                            stopWorkClient = true;
                            return;
                        }

                        continue;
                    }

                    nopeIterval = false;

                    int workTime = time.elapsed();
                    setSpeed((double)crack->getStep() / workTime);

                    QString pass = command.mid(4, command.length() - 4);


                    crack->setCalcOnClient(interval);
                    crack->setAnswer(pass);

                    continue;
                }
            }

            if (stopWorkClient && nopeIterval)
                crack->setNeedCalc(interval);

            if(!sendAll("Failed command\r\n"))
            {
                if(nopeIterval)
                    crack->setNeedCalc(interval);
                erroeStopClient("Отключен " + clientAddress);
                stopWorkClient = true;
                return;
            }
        }
    }
}

bool SocketClient::sendAll(QString mass)
{
    int len = mass.length();
    QByteArray array = mass.toLocal8Bit();
    char* buffer = array.data();

    int n;
    int total = 0;
    int bytesleft = len;

    while(total < len)
    {
        n = send(clientSocket, buffer+total, bytesleft, 0);

        if (n == -1)
            break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? false : true;
}

bool SocketClient::sendFile()
{
   // "netDoc.docx"
    QFile fileNet("netDoc.docx");

    if (!fileNet.exists())
    {
        if(!QFile::copy(crack->fileName, "netDoc.docx"))
        {
            emit criticalErrorSignal();
        }
    }

    if(!fileNet.open(QIODevice::ReadOnly))
    {
        return false;
    }

    qint64 len = fileNet.size();
    QByteArray block = fileNet.readAll();

    if(!sendAllByte(block, len))
    {
        return false;
    }


    fileNet.close();
    block.clear();

    return true;
}

bool SocketClient::sendAllByte(QByteArray block, qint64 len)
{
    char* buffer = block.data();

    qint64 n;
    qint64 total = 0;
    qint64 bytesleft = len;

    while(total < len)
    {

        //QFile f("supet.docx");



        n = send(clientSocket, buffer+total, bytesleft, 0);

        if (n == -1)
            break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? false : true;
}

bool SocketClient::getMassege()
{
    struct timeval timeout;
    struct fd_set fds;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    char buff[129];
    memset(buff, '\0', sizeof(buff));
    QString possibleTeam = lastMass;

    int timerCount = 0;

    do
    {
        FD_ZERO(&fds);
        FD_SET(clientSocket, &fds);

        int resultSelect = select(0, &fds, 0, 0, &timeout);

        if(resultSelect < 0)
            return false;

        if(resultSelect == 1)
        {
            int n = recv(clientSocket, buff, 128, 0);

            if(n <= 0)
                return false;

            timerCount = 0;
            possibleTeam += QString::fromUtf8(buff);

            int w = possibleTeam.indexOf('\n');
            int l = possibleTeam.length();

            if(possibleTeam.length() > 128)
            {
                possibleTeam = possibleTeam.mid(64, possibleTeam.length() - 64);
            }
        }

        if (resultSelect == 0)
            timerCount++;

        if(timerCount > TimeOut * 10)
            return false;
    }
    while (possibleTeam.indexOf('\n') + 1 != possibleTeam.length() || possibleTeam.length() == 0);

    QStringList pars = possibleTeam.split("\r\n", QString::SkipEmptyParts);
    for (int i = 0; i < pars.size(); i++)
        commandQueue.enqueue(pars.at(i));

    return true;

}

void SocketClient::setSpeed(double speed)
{
    crack->mutexForSpeedMap.lock();
    crack->speedMap[id] = speed * 1000;
    crack->mutexForSpeedMap.unlock();
}

void SocketClient::removeSpeed()
{
    crack->mutexForSpeedMap.lock();
    crack->speedMap.remove(id);
    crack->mutexForSpeedMap.unlock();
}

