#include "cracker.h"

Cracker::Cracker(QObject *parent)
{
    initialization();
}

Cracker::~Cracker()
{
    if (word != NULL)
        word->dynamicCall("Quit()");



    if (document != NULL)
        delete document;

    if (word != NULL)
        delete word;
}

bool Cracker::getPasswordInterval(int interval[])
{
    mutexForInterval.lock();
    if (lastPass == maxVariant)
    {
        mutexForInterval.unlock();
        stopCalculating = true;
        return false;
    }
    interval[0] = lastPass;
    lastPass += passwordStep;
    if (lastPass > maxVariant)
    {
        lastPass = maxVariant;
    }
    interval[1] = lastPass;
    mutexForInterval.unlock();
    return true;

}

void Cracker::erroeStop(QString errorMy)
{
    emit sendMassegeSignal(errorMy);
    stopCalculating = true;

    if (word != NULL)
        word->dynamicCall("Quit()");



    if (document != NULL)
        delete document;

    if (word != NULL)
        delete word;

}

void Cracker::stop()
{
    stopCalculating = true;
    erroeStop("Процесс остановлен");
}

void Cracker::process()
{
    emit sendMassegeSignal("Начало работы");
    QThread::usleep(5);

    stopCalculating = false;

    if (stopCalculating)
    {
        emit finished();
        return;
    }
    if(!checkFile())
    {
        erroeStop("Файл не обнаружен");
        emit finished();
        return;
    }

    if (stopCalculating)
    {
        emit finished();
        return;
    }
    if(!myCopyFile())
    {
        erroeStop("Не удалось копирование");
        emit finished();
        return;
    }

    emit sendMassegeSignal("Файл скопирован");
    QThread::usleep(5);

    if (stopCalculating)
    {
        emit finished();
        return;
    }
    word = new QAxObject("Word.Application");
    if (word == NULL)
    {
        erroeStop("Не удалось открыть Word");
        emit finished();
        return;
    }

    ExceptReceiver *er = new ExceptReceiver();
    word->setProperty("Visible", true);
    QObject::connect(word,SIGNAL(exception(int, QString, QString, QString)),
                     er,SLOT(debugError(int,QString,QString,QString)));

    emit sendMassegeSignal("Word открыт");
    QThread::usleep(5);

    if (stopCalculating)
    {
        emit finished();
        return;
    }
    document = word->querySubObject("Documents");
    if (word == NULL)
    {
        erroeStop("Не удалось открыть список документов");
        emit finished();
        return;
    }

    emit sendMassegeSignal("Открыт список документов Word");
    QThread::usleep(5);

}

void Cracker::initialization()
{
    fileName.clear();
    netFileName = "netDoc.docx";
    word = 0;
    document = 0;
    stopCalculating = false;
    answer.clear();

    lastPass = 0;
    maxVariant = qPow(AlphabetLen, PassLen);

    needCalc.clear();
    calcOnClient.clear();

    for (int i = 0; i < AlphabetLen; i++)
    {
        if (i < 10)
        {
            alphabet[i] = (char)(i + 48);
        }
        else
        {
            alphabet[i] = (char)(i + 87);
        }
    }

    QFile file(netFileName);
    if(file.exists())
        file.remove();

    bitMasck = 3;
    passwordStep = maxVariant * 0.0000001;
}

bool Cracker::checkFile()
{
    QFile file1(fileName);

    if (!file1.exists())
        return false;

    file1.close();
    return true;
}

bool Cracker::myCopyFile()
{
    return QFile::copy(fileName, netFileName);
}


