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
    qDebug() << checkFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialization()
{
    fileName.clear();
    netFileName.clear();
    word = 0;
    document = 0;
    stopCalculating = false;

    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

    ui->pushButton_Close->setDisabled(true);

    for (int i = 0; i < PassLen; i++)
        lastPass[i] = '\0';

    needCalc.clear();

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
}

bool MainWindow::checkFile()
{
    QFile file1(fileName);

    if (!file1.exists())
        return false;

    file1.close();
    return true;
}

void MainWindow::erroeStop(QString errorMy)
{
    ui->textEdit->append(errorMy);
}

bool MainWindow::myCopyFile()
{
    return QFile::copy(fileName, "netDoc.docx");
}

void MainWindow::sendMassege(QString mass)
{
    ui->textEdit->append(mass);
}

void MainWindow::OpenWord()
{
    QAxObject* word = new QAxObject("Word.Application");
    ExceptReceiver er;
    word->setProperty("Visible", true);
    QObject::connect(word,SIGNAL(exception(int, QString, QString, QString)),
                     &er,SLOT(debugError(int,QString,QString,QString)));
    QAxObject* document = word->querySubObject("Documents");
    QObject::connect(word,SIGNAL(exception(int, QString, QString, QString)),
                     &er,SLOT(debugError(int,QString,QString,QString)));
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

        if(workDoc != 0 || stopCalculating)
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
    ui->pushButton_Close->setEnabled(true);
    ui->textEdit->clear();

    if(!checkFile())
    {
        erroeStop("Файл не обнаружен");
        return;
    }

    if(!myCopyFile())
    {
        erroeStop("Не удалось копирование");
        return;
    }

    sendMassege("Файл скопирован");
}

void MainWindow::on_pushButton_Close_clicked()
{
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_Close->setDisabled(true);
    ui->pushButton_Open->setEnabled(true);
}

void MainWindow::on_pushButton_Open_clicked()
{
    fileName = QFileDialog::getOpenFileName(
                this,
                "Open File",
                "C://",
                "Word Documents (*.docx)"
                );
    if (fileName == "")
        return;

    ui->lineEdit->setText(fileName);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_Open->setDisabled(true);
}
