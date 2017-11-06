#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QTextBrowser>

class OutputThread : public QThread
{
    Q_OBJECT

public:
    OutputThread(QTextBrowser *edit);
    void setText(QString mass);
    void run ();

private:
    QTextBrowser *edit;
    QString mass;
};

#endif // OUTPUTTHREAD_H
