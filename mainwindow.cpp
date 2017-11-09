#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exceptreceiver.h"

//-2146822880
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialization();
    crac = new Cracker(nullptr);
    serv = new SocketServer(nullptr, crac);

    connect(crac, SIGNAL(sendMassegeSignal(QString)), this, SLOT(sendMassege(QString)));
    connect(serv, SIGNAL(sendNetMassegeSignal(QString)), this, SLOT(sendNetMassege(QString)));
    //OpenWord();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialization()
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
}


void MainWindow::sendMassege(QString mass)
{
    ui->textBrowser->append(mass);
}

void MainWindow::sendNetMassege(QString mass)
{
    ui->textBrowser_Net->append(mass);
}


void MainWindow::OpenWord()
{
    QAxObject* word = new QAxObject("Word.Application");
    ExceptReceiver er;
    word->setProperty("Visible", true);
    QAxObject* document = word->querySubObject("Documents");
    QString filename = "F:\\QtProject\\WordWork\\WordWork\\test.docx";
    QVariant confirmconversions(false);
    QVariant readonly(true);
    QVariant addtorecentfiles(false);
    QVariant passwordtemplate("");
    QVariant revert(false);



    for (int i = 11; i < 20; i++)
    {
        QString passworddocument = QString::number(i);
        QAxObject* workDoc;
        workDoc = document->querySubObject("Open(const QString&, const QVariant&, "
                                          "const QVariant&, const QVariant&, const QString&, const QVariant&,"
                                          "const QVariant&)", filename, confirmconversions, readonly,
                                          addtorecentfiles, passworddocument, passwordtemplate, revert);


            QObject::connect(word,SIGNAL(exception(int, QString, QString, QString)),
                             &er,SLOT(debugError(int,QString,QString,QString)));

        if(workDoc != 0)
        {
            word->querySubObject("ActiveDocument")->dynamicCall("Close()");
            delete workDoc;
            break;
        }
    }


    word->dynamicCall("Quit()");

    delete document;
    delete word;
}




void MainWindow::on_pushButton_2_clicked()
{
    ui->pushButton_2->setDisabled(true);
    ui->textBrowser->clear();

    if (crac->fileName == "")
        crac->fileName = ui->lineEdit->text();

    QThread *thread = new QThread;
    QThread *threadNet = new QThread;

    crac->moveToThread(thread);
    serv->moveToThread(threadNet);

    connect(thread, SIGNAL(started()), crac, SLOT(process()));

    connect(crac, SIGNAL(finished()), thread, SLOT(quit()));

    connect(this, SIGNAL(stopAll()), crac, SLOT(stop()));

    connect(crac, SIGNAL(finished()), crac, SLOT(deleteLater()));

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));




    connect(threadNet, SIGNAL(started()), serv, SLOT(processNet()));

    connect(serv, SIGNAL(finishedNet()), threadNet, SLOT(quit()));

    connect(this, SIGNAL(stopAll()), serv, SLOT(stopNet()));

    connect(serv, SIGNAL(finishedNet()), serv, SLOT(deleteLater()));

    connect(threadNet, SIGNAL(finished()), threadNet, SLOT(deleteLater()));

    //connect(crac, SIGNAL(finished()), crac, SLOT(stop()));
    thread->start();
    threadNet->start();
    //thread->wait();

    return;

}

void MainWindow::on_pushButton_Close_clicked()
{
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_Open->setEnabled(true);

    emit stopAll();
}

void MainWindow::on_pushButton_Open_clicked()
{
    crac->fileName = QFileDialog::getOpenFileName(
                this,
                "Open File",
                "C://",
                "Word Documents (*.docx)"
                );
    if (crac->fileName == "")
        crac->fileName = ui->lineEdit->text();

    if (crac->fileName == "")
        return;

    ui->lineEdit->setText(crac->fileName);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_Open->setDisabled(true);
}


