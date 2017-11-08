#include "outputthread.h"

OutputThread::OutputThread(QObject *parent)
{
}

void OutputThread::setText(QString mass)
{
    this->mass = mass;
}

void OutputThread::run()
{
    //edit->insertPlainText(mass + '\n');
}
