#include "outputthread.h"

OutputThread::OutputThread(QTextBrowser *edit)
{
    this->edit = edit;
}

void OutputThread::setText(QString mass)
{
    this->mass = mass;
}

void OutputThread::run()
{
    edit->append(mass);
}
