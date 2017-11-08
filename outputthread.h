#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QTextBrowser>

class OutputThread : public QThread
{
    Q_OBJECT

public:
    OutputThread(QObject *parent = nullptr);
    void setText(QString mass);
    void run ();

private:
    QObject edit;
    QString mass;
};

#endif // OUTPUTTHREAD_H
