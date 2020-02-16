#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QBluetoothTransferRequest>
#include <QBluetoothTransferReply>
#include <QFileDialog>

#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>

static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
//PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);
//void CreateBMPFile(LPSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);
//void takeScreenshot(int width, int height);
QBluetoothAddress adddr;
char *FileName1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    const QBluetoothAddress adapter = QBluetoothAddress();
                                          // localAdapters.at(currentAdapterIndex).address();


     m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapter);

     connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
             this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
     connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
     connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(discoveryFinished()));


     server = new ChatServer(this);
     connect(server, QOverload<const QString &>::of(&ChatServer::clientConnected),
             this, &MainWindow::clientConnected);


     /*


     connect(server, QOverload<const QString &>::of(&ChatServer::clientDisconnected),
             this,  QOverload<const QString &>::of(&MainWindow::clientDisconnected));*/
     connect(server, &ChatServer::messageReceived,
             this,  &MainWindow::showMessage);

     connect(this, &MainWindow::sendMessage, server, &ChatServer::sendMessage);


     server->startServer();

//system("D:\\minicap\\MiniCap.exe -capturedesktop -exit -save \"desktop.png\" -closeapp -exit ");
//system("D:\\minicap\\MiniCap.exe -capturedesktop -exit -save \"D:\\desktop.png\" -closeapp -exit ");
//system("D:\\minicap\\MiniCap.exe");
//system("D:\\minicap\\MiniCap.exe");


     localName = QBluetoothLocalDevice().name();
    // qDebug() << "I am here2"<< localName;




}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_pushButton_clicked()
{


    if (m_discoveryAgent->isActive())
        m_discoveryAgent->stop();

    ui->listWidget->clear();

    m_discoveryAgent->setUuidFilter(QBluetoothUuid(serviceUuid));
    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

}

QBluetoothServiceInfo MainWindow::service() const
{
    return m_service;
}


void MainWindow::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();


    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
    while (i.hasNext()){
        i.next();
        if (serviceInfo.device().address() == i.value().device().address()){
            return;
        }
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = serviceInfo.device().address().toString();
    else
        remoteName = serviceInfo.device().name();

    QListWidgetItem *item =
        new QListWidgetItem(QString::fromLatin1("%1 %2").arg(remoteName,
                                                             serviceInfo.serviceName()));

    m_discoveredServices.insert(item, serviceInfo);
    ui->listWidget->addItem(item);
}


void MainWindow::showMessage(const QString &sender, const QString &message)
{
   // ui->chat->insertPlainText(QString::fromLatin1("%1: %2\n").arg(sender, message));
   // ui->chat->ensureCursorVisible();
    qDebug() << message;
    // Тут обработка всего что приходит с сервера/клиента
    // Можем использовать emit для отправки сигнала с передачей всех нужных байтов, они будут отправлены на
    // клиент/сервер.



  /*  if (message[0] == '0') // Запрос скрина с текущего сервера
    {
        system("D:\\minicap\\MiniCap.exe -capturedesktop -exit -save \"D:\\desktop.png\" -closeapp -exit ");


        //QString sendM;
   /*     QFile file("D:\\desktop.png");
            if (!file.open(QIODevice::ReadOnly))
                exit(2);
            QString sendM; //= file.readAll();
            sendM.append('2');
            char tmp[30];
            memset(tmp, 0, 30);
            itoa(file.size(), tmp, 10);
            sendM.append(tmp);
            sendM.append('@');
            int i=0;
            while(i < file.size())
            {
                char c;
                file.getChar(&c);// += file.readAll();
                sendM.append(c);
                i++;
               // if(i> 100)break;
            }
            file.close();
            emit sendMessage(sendM);*/



      /*  QBluetoothTransferManager mgr;
        QBluetoothTransferRequest req(adddr);

        QFile *m_file = new QFile("D:\\desktop.png");

        QBluetoothTransferReply *reply = mgr.put(req, m_file);
        //mgr is default parent
        //ensure that mgr doesn't take reply down when leaving scope
        reply->setParent(this);
        if (reply->error()){
            qDebug() << "Failed to send file";
            reply->deleteLater();
            return;
        }
*/


        /*FILE *scrn;
        char name[] = "D:\\desktop.png";
         if ((scrn = fopen(name, "r")) == NULL)
         {
           qDebug("Не удалось открыть файл");
           getchar();
           exit(0);
         }
         QString sendM;
         char cd;
         int i = 1;
         //sendM[0] = '2';
         sendM.append('2');
        /* fscanf(scrn, "%c", &cd);
         while(cd != EOF)
         {
             //sendM[i] = c;
             sendM.append(cd);
             //sendM.append(cd);
             //sendM.a
             //i++;
              fscanf(scrn, "%c", &cd);
         }*/

        /* do
         {
             // Taking input single character at a time
             char c = fgetc(scrn);

             // Checking for end of file
             if (feof(scrn))
                 break ;

            sendM.append(c);
         }  while(1);


         //sendM[i] = '\0';
         fclose(scrn);*/
/*
    }
    else if (message[0] == '1') // Запрос файла с текущего сервера
    {
        QString filename;
        int i = 1;
        while (message[i] != '@')
        {
            filename[i - 1] = message[i];
            i++;
        }
    }
    else if(message[0] == '2') // Получение скрина с текущего сервера
    {
        qDebug() << message.size();
        QFile file("C:\\Qt\\screenshots\\desktop.png");
            if (!file.open(QIODevice::ReadWrite))
                exit(1);
            QString sendM; //= file.readAll();
            sendM.append('2');
            int i=0;
            while(i < file.size())
            {
                char c;
                file.getChar(&c);// += file.readAll();
                sendM.append(c);
                i++;
               // if(i> 100)break;
            }
            file.close();
            emit sendMessage(sendM);

        FILE *scrn;
        char name[] = "C:\\Qt\\screenshots\\desktop.png";
         if ((scrn = fopen(name, "w")) == NULL)
         {
           qDebug("Не удалось открыть файл");
           getchar();
           exit(0);
         }
         QString mes1;
         //int i=0;
         while (i < message.size())
         {
             mes1.append(message[i]);
             i++;
         }
        // mes1[i - 1] = '\0';
         fprintf(scrn, "%s", mes1.toUtf8().data());
         fclose(scrn);
         //std::ofstream scrn("output.txt");
        // scrn << mes1;
         system("start C:\\Qt\\screenshots\\desktop.png");
    }



*/

}

void MainWindow::on_pushButton_3_clicked() // запрос файла
{


    QString oneFilename = "1" + ui->lineEdit_2->text();

    qInfo() << QString(oneFilename);

    clients[0]->first_pack = true;
    clients[0]->pack_counter = 0;
    ui->progressBar->setValue(0);
    emit sendMessage(oneFilename);
}

void MainWindow::on_pushButton_4_clicked() // запрос скрина
{
    emit sendMessage("0");
}

void MainWindow::connect_to() // Подключение
{
    QBluetoothServiceInfo service = this->service();
    qDebug() << "Connecting to service 2" << service.serviceName()
             << "on" << service.device().name();

    // Create client
    qDebug() << "Going to create client";
    ChatClient *client = new ChatClient(this);
    //client->main = this;
    qDebug() << "Connecting...";

    connect(client, &ChatClient::messageReceived,
            this, &MainWindow::showMessage);
   /* connect(client, &ChatClient::disconnected,
            this, QOverload<>::of(&Chat::clientDisconnected));
    connect(client, QOverload<const QString &>::of(&ChatClient::connected),
            this, &Chat::connected);
    connect(client, &ChatClient::socketErrorOccurred,
            this, &Chat::reactOnSocketError);*/
    connect(this, &MainWindow::sendMessage, client, &ChatClient::sendMessage);
    qDebug() << "Start client";

    ui->listWidget->clear();
    QListWidgetItem *item =
    new QListWidgetItem(QString::fromLatin1("%1 connected").arg( service.serviceName()));
    adddr = service.device().address();
    //m_discoveredServices.insert(item, serviceInfo);
    ui->listWidget->addItem(item);


    client->startClient(service);

    connect(client, &ChatClient::update_status,
            this, &MainWindow::update_bar);

    connect(client, &ChatClient::write,
            this, &MainWindow::write);

    clients.append(client);




}


void MainWindow::clientConnected(const QString &name)
{
   // ui->chat->insertPlainText(QString::fromLatin1("%1 has joined chat.\n").arg(name));
   // ui->listWidget.ins
        ui->listWidget->clear();
    QListWidgetItem *item =
        new QListWidgetItem(QString::fromLatin1("%1 connected").arg(name));
 //adddr = adr;
    //m_discoveredServices.insert(item, serviceInfo);
    ui->listWidget->addItem(item);
}

void MainWindow::on_listWidget_itemActivated(QListWidgetItem *item)
{
    qDebug() << "got click" << item->text();
    m_service = m_discoveredServices.value(item);
    if (m_discoveryAgent->isActive())
        m_discoveryAgent->stop();
    this->connect_to();
    //accept();
}


void MainWindow::on_lineEdit_editingFinished()
{

    //memcpy(name_file, ui->lineEdit->text().toUtf8(), ui->lineEdit->text().size());
    /*FileName1 = (char *)malloc(ui->lineEdit->text().size() * sizeof(char) + 1);
    for (int x=0; x < ui->lineEdit->text().size(); x++)
    {
        FileName1[x] =  name_file[x];
    }
    FileName1[ui->lineEdit->text().size()]= '\0';*/
}

void MainWindow::on_pushButton_3_toggled(bool checked)
{

}

void MainWindow::update_bar(int num)
{
    ui->progressBar->setValue(num);
}
void MainWindow::write(QByteArray src, bool del)
{
    if(del)
        received_sh+=src;
    else
    {
        received_sh.clear();
        received_sh += src;
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                         "C:/Python27/untitled.txt",
                                                         tr("Text files (*.txt)"));
    QFile f( filename );
    f.open( QIODevice::WriteOnly );

    f.write(received_sh);
    f.close();
}
