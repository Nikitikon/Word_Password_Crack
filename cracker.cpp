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

bool Cracker::getPasswordInterval(quint64 interval[])
{
    mutexForInterval.lock();
    if (lastPass == maxVariant)
    {
        mutexForInterval.unlock();
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

bool Cracker::setCalcOnClient(quint64 interval[])
{
    mutexForClientCalcList.lock();

    for (int i = 0; i < 1; i++)
    {
        if(interval[i] < 0)
        {
            interval[i] = 0;
        }

        if(interval[i] > maxVariant)
        {
            interval[i] = maxVariant;
        }
    }

    QString dataToList = converInterval(interval);

    if (dataToList == "")
    {
        mutexForClientCalcList.unlock();
        return false;
    }

    if (calcOnClient.contains(dataToList))
    {
        mutexForClientCalcList.unlock();
        return true;
    }

    calcOnClient.enqueue(dataToList);
    mutexForClientCalcList.unlock();
    return true;
}

bool Cracker::setNeedCalc(quint64 interval[])
{
    mutexForNeedList.lock();

    for (int i = 0; i < 1; i++)
    {
        if(interval[i] < 0)
        {
            interval[i] = 0;
        }

        if(interval[i] > maxVariant)
        {
            interval[i] = maxVariant;
        }
    }

    QString dataToList = converInterval(interval);

    if (dataToList == "")
    {
        mutexForNeedList.unlock();
        return false;
    }

    if (needCalc.contains(dataToList))
    {
        mutexForNeedList.unlock();
        return true;
    }

    needCalc.push(dataToList);
    mutexForNeedList.unlock();
    return true;
}

bool Cracker::setAnswer(QString assumedKey)
{
    mutexForAnswerList.lock();

    if (assumedKey.length() > QString::number(maxVariant).length())
    {
        mutexForAnswerList.unlock();
        return false;
    }

    if(answer.contains(assumedKey))
    {
        mutexForAnswerList.unlock();
        return true;
    }

    answer.enqueue(assumedKey);

    mutexForAnswerList.unlock();

    return true;
}

bool Cracker::getCalcOnClient(quint64 interval[])
{
    mutexForClientCalcList.lock();

    if (calcOnClient.isEmpty())
    {
        mutexForClientCalcList.unlock();
        return false;
    }

    QString temp = calcOnClient.dequeue();

    if(!converString(temp, interval))
    {
        mutexForClientCalcList.unlock();
        return false;
    }

    mutexForClientCalcList.unlock();
    return true;
}

bool Cracker::getNeedCalc(quint64 interval[])
{
    mutexForNeedList.lock();

    if(needCalc.isEmpty())
    {
        mutexForNeedList.unlock();
        return false;
    }

    QString temp = needCalc.pop();

    if(!converString(temp, interval))
    {
        mutexForNeedList.unlock();
        return false;
    }

    mutexForNeedList.unlock();
    return true;
}

QString Cracker::getTryAnswer()
{
    mutexForAnswerList.lock();

    if(answer.isEmpty())
    {
        mutexForAnswerList.unlock();
        return false;
    }

    QString temp = answer.dequeue();

    mutexForAnswerList.unlock();
    return temp;
}

void Cracker::erroeStop(QString errorMy)
{
    emit sendMassegeSignal(errorMy);
    stopCalculating = true;

    QThread::usleep(10);

    if (word != NULL)
        word->dynamicCall("Quit()");



    if (document != NULL)
    {
        delete document;
        document = 0;
    }

    if (word != NULL)
    {
        delete word;
        word = 0;
    }

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

    QApplication::processEvents();
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

    QApplication::processEvents();
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

    QApplication::processEvents();
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

    QApplication::processEvents();
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

    QApplication::processEvents();
    crackPassword();

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
    passwordStep = 50;
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

QString Cracker::converInterval(quint64 interval[])
{
    QString convert = "";
    for (int i = 0; i < 1; i++)
        if(interval[i] < 0 || interval[i] > maxVariant)
            return convert;
    convert = QString::number(interval[0]) + "&" + QString::number(interval[1]);
    return convert;
}

bool Cracker::converString(QString str, quint64 interval[])
{
    if(str == "" || !str.contains('&'))
        return false;

    QString tempFirst = str.mid(0, str.indexOf('&'));
    QString tempSecond = str.mid(str.indexOf('&') + 1, str.length());

    if (tempFirst == "" || tempSecond == "")
        return false;

    try{
        interval[0] = (quint64)tempFirst.toLongLong();
        interval[1] = (quint64)tempSecond.toLongLong();

        if (interval[0] > interval[1])
        {
            int temp = interval[0];
            interval[0] = interval[1];
            interval[1] = temp;
        }
    }
    catch(QException){
        return false;
    }
    return true;
}

bool Cracker::fromClientCalcToNeedCalc()
{
    mutexForClientCalcList.lock();
    mutexForNeedList.lock();
    QString temp = "";

    if(calcOnClient.isEmpty())
    {
        return false;
    }

    while (calcOnClient.isEmpty())
    {
        temp = calcOnClient.dequeue();
        needCalc.push(temp);
    }

    mutexForClientCalcList.unlock();
    mutexForNeedList.unlock();

    return true;
}

QString Cracker::createPass(int step)
{
    QString temp = "";

    while (step >= AlphabetLen) {
        temp += alphabet[step % AlphabetLen];
        step = step / AlphabetLen;
    }
    temp += alphabet[step];

    return temp;
}

bool Cracker::crackPassword()
{
    QApplication::processEvents();
    emit sendMassegeSignal("Взлом начат...");
    QThread::usleep(5);

    QVariant confirmconversions(false);
    QVariant readonly(true);
    QVariant addtorecentfiles(false);
    QVariant passwordtemplate("");
    QVariant revert(false);

    QString passworddocument = "";

    QAxObject* workDoc;

    int count = 0;

    while (!stopCalculating) {

        QThread::usleep(5);

        if(!checkFile())
        {
            erroeStop("Файл не обнаружен");
            stop();
            return false;
        }

        passworddocument = getTryAnswer();
        if(passworddocument != "" && !stopCalculating)
        {
            QApplication::processEvents();

            workDoc = document->querySubObject("Open(const QString&, const QVariant&, "
                                              "const QVariant&, const QVariant&, const QString&, const QVariant&,"
                                              "const QVariant&)", fileName, confirmconversions, readonly,
                                              addtorecentfiles, passworddocument, passwordtemplate, revert);
            if (workDoc != 0)
            {
                sendMassegeSignal("Пароль" + passworddocument);
                word->querySubObject("ActiveDocument")->dynamicCall("Close()");
                delete workDoc;
                stop();
                return true;
            }

            continue;
        }

        quint64 interval[2];

        if(getNeedCalc(interval) && !stopCalculating)
        {
            for (int i = 0; i < passwordStep; i++)
            {
                QApplication::processEvents();

                QThread::usleep(5);
                passworddocument = createPass(interval[0]);
                interval[0]++;
                workDoc = document->querySubObject("Open(const QString&, const QVariant&, "
                                                  "const QVariant&, const QVariant&, const QString&, const QVariant&,"
                                                  "const QVariant&)", fileName, confirmconversions, readonly,
                                                  addtorecentfiles, passworddocument, passwordtemplate, revert);
                if (workDoc != 0)
                {
                    sendMassegeSignal("Пароль" + passworddocument);
                    word->querySubObject("ActiveDocument")->dynamicCall("Close()");
                    delete workDoc;
                    stop();
                    return true;
                }
            }

            continue;
        }

        if(getPasswordInterval(interval) && !stopCalculating)
        {
            for (int i = 0; i < passwordStep; i++)
            {
                QApplication::processEvents();

                if(stopCalculating) break;
                QThread::usleep(1);
                passworddocument = createPass(interval[0]);
                interval[0]++;
                workDoc = document->querySubObject("Open(const QString&, const QVariant&, "
                                                  "const QVariant&, const QVariant&, const QString&, const QVariant&,"
                                                  "const QVariant&)", fileName, confirmconversions, readonly,
                                                  addtorecentfiles, passworddocument, passwordtemplate, revert);
                //sendMassegeSignal(passworddocument);

                if (workDoc != 0)
                {
                    sendMassegeSignal("Пароль" + passworddocument);
                    word->querySubObject("ActiveDocument")->dynamicCall("Close()");
                    delete workDoc;
                    stop();
                    return true;
                }
            }

            continue;
        }


        if(getCalcOnClient(interval) && count >= 6)
        {
            for (int i = 0; i < passwordStep; i++)
            {
                QThread::usleep(5);
                QApplication::processEvents();

                passworddocument = createPass(interval[0]);
                interval[0]++;
                workDoc = document->querySubObject("Open(const QString&, const QVariant&, "
                                                  "const QVariant&, const QVariant&, const QString&, const QVariant&,"
                                                  "const QVariant&)", fileName, confirmconversions, readonly,
                                                  addtorecentfiles, passworddocument, passwordtemplate, revert);
                if (workDoc != 0)
                {
                    sendMassegeSignal("Пароль" + passworddocument);
                    word->querySubObject("ActiveDocument")->dynamicCall("Close()");
                    delete workDoc;
                    stop();
                    return true;
                }
            }

            count--;

            if(fromClientCalcToNeedCalc())
            {
                count = 0;
                continue;
            }
        }

        QThread::sleep(5);
        count++;

        if(count == 6)
        {
            delete workDoc;
            sendMassegeSignal("Пароль не найден");
            stop();
            return false;
        }
    }
    return true;
}



