/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chatserver.h"
#include <malloc.h>
#include <QDebug>
#include <dos.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QtBluetooth/qbluetoothserver.h>
#include <QtBluetooth/qbluetoothsocket.h>
#include <QBluetoothLocalDevice>

//! [Service UUID]
static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
//! [Service UUID]

ChatServer::ChatServer(QObject *parent)
    :   QObject(parent)
{
}

ChatServer::~ChatServer()
{
    stopServer();
}

void ChatServer::startServer(const QBluetoothAddress& localAdapter)
{
    if (rfcommServer)
        return;

    QBluetoothLocalDevice a(this);

    qInfo() << a.address();

    //! [Create the server]
    //!
    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, &QBluetoothServer::newConnection,
            this, QOverload<>::of(&ChatServer::clientConnected));
    bool result = rfcommServer->listen(QBluetoothAddress("5C:5F:67:92:95:E5"));
    if (!result) {
        qWarning() << "Cannot bind chat server to" << localAdapter.toString();
        return;
    }
    //! [Create the server]

    //serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

    QBluetoothServiceInfo::Sequence profileSequence;
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
    classId << QVariant::fromValue(quint16(0x100));
    profileSequence.append(QVariant::fromValue(classId));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BluetoothProfileDescriptorList,
                             profileSequence);

    classId.clear();
    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));

    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    //! [Service name, description and provider]
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Chat Server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("Example bluetooth chat server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("qt-project.org"));
    //! [Service name, description and provider]

    //! [Service UUID set]
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             publicBrowse);
    //! [Service Discoverability]

    //! [Protocol descriptor list]
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);
    //! [Protocol descriptor list]

    //! [Register service]
    serviceInfo.registerService(localAdapter);
    //! [Register service]
    qDebug() << "I am here"<< localAdapter;
    // F8-A2-D6-46-C4-E6
}

//! [stopServer]
void ChatServer::stopServer()
{
    // Unregister service
    serviceInfo.unregisterService();

    // Close sockets
    qDeleteAll(clientSockets);

    // Close server
    delete rfcommServer;
    rfcommServer = nullptr;
}
//! [stopServer]

//! [sendMessage]
void ChatServer::sendMessage(const QString &message)
{
    QByteArray text = message.toUtf8() + '\n';

    for (QBluetoothSocket *socket : qAsConst(clientSockets))
        socket->write(text);



}
//! [sendMessage]

//! [clientConnected]
void ChatServer::clientConnected()
{
    QBluetoothSocket *socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;

    connect(socket, &QBluetoothSocket::readyRead, this, &ChatServer::readSocket);
    connect(socket, &QBluetoothSocket::disconnected, this, QOverload<>::of(&ChatServer::clientDisconnected));
    clientSockets.append(socket);
    emit clientConnected(socket->peerName());

    //QBluetoothAddress as;
    //as = socket->peerAddress();

/*
    QFile file("D:\\desktop.png");
                if (!file.open(QIODevice::ReadOnly))
                    exit(2);
                char sendM[1000000]; //= file.readAll();
                sendM[0] = '2';
                char tmp[30];
                memset(tmp, 0, 30);
                itoa(file.size(), tmp, 10);
                //sendM.append('@');
                int i=0;
                while(i < file.size())
                {
                    char c;
                    file.getChar(&c);// += file.readAll();
                    sendM[i + 1] = c;
                    i++;
                   // if(i> 100)break;
                }
                sendM[i + 1] = '\0';
                AF_BTH = socket1->peerAddress();
LocalSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (SOCKET_ERROR == send(LocalSocket,
                    (char*)sendM,
                    file.size(),
                    0))
                {
               //     outputMut.lock();
                    qDebug(L"=CRITICAL= | WSAGetLastError=[%d]\n", WSAGetLastError());
               //     outputMut.unlock();
               //     ulRetCode = CXN_ERROR;
               //     break;
                }
     file.close();*/

}
//! [clientConnected]

//! [clientDisconnected]
void ChatServer::clientDisconnected()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;

    emit clientDisconnected(socket->peerName());

    clientSockets.removeOne(socket);

    socket->deleteLater();
}
//! [clientDisconnected]

//! [readSocket]
void ChatServer::readSocket()
{
    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    if (!socket)
        return;
    //QByteArray line = socket->readLine().trimmed();
    char line[2];
    socket->read(line, 1);
    if(line[0] == '0')
    {
       FILE* input = NULL;
       system("C:\\minicap\\MiniCap.exe -capturedesktop -exit -save \"C:\\minicap\\desktop.png\" -closeapp -exit");
       input = fopen("C:\\minicap\\desktop.png", "rb");
       rewind(input);

       fseek(input, 0L, SEEK_END);
       long long sizeFile = ftell(input);
       if (sizeFile <= 0)
       {
           exit(3);
       }
       //long long sz = sizeFile + 4 + 4;
       //seek back:
       fseek(input, 0L, SEEK_SET);

       unsigned long long i=0;
       char *socksend = NULL;
      // char *socksend1 = NULL;
       socksend = (char*)malloc(sizeof(char) * (sizeFile));
       memset(socksend, 0, sizeFile);
      // socksend1 = (char*)malloc(sizeof(char) * (sizeFile));
       //socksend[0] = '2';
       char r[2];
       long long tt=0;
     //  while(1)
     //  {
            long long t = fread((void*)&socksend[0], sizeof(char), sizeFile, input);

           /* for(int n=0; n<100; n++) qDebug("%i", socksend[n]);
            if ((t + tt) < sizeFile)
            {
                fseek(input, t+tt+1, 0);
                socksend[tt + t] = 0;
                for(int iu = tt; iu< t+tt; iu++)
                {
                    socksend[iu] = socksend1[iu - tt];
                }
                tt += t;
            }
            else break;*/

      // }

      // if (feof(input))
      // qDebug("Ошибка чтения test.bin: неожиданный конец файла ");
      /* while(i < sizeFile)
       {
            r[0] = fgetc(input);

            if (i < sizeFile)
            socksend[i] = r[0];
            i++;
       }*/

       //for (QBluetoothSocket *socket : qAsConst(clientSockets)) socket->write(socksend);
       //long c = socket->write(socksend, sizeFile);
        QByteArray filesize;

        socket->write(filesize.setNum(sizeFile));
        i=0;
       char lin[9000];
       memset(lin, 0, 9000);
       lin[1] = 0;
       int u=0;
       while(i * 9000 < sizeFile)
       {
           u=0;
           while ((u < 9000) && ((u + i*9000) < sizeFile))
           {
               lin[u] = socksend[i* 9000 + u];
               u++;
           }
            Sleep(1000);
            socket->write(lin, u);
            i++;
       }
    }
    else if(line[0] == '1')
    {
       // FILE* input = NULL;
        //input = fopen("C:\\Qt\\screenshots\\desktop.png", "wb");
       // rewind(input);

        int t = 0, count_t = 0;
        QString FilenameSock;

        FilenameSock += socket->readAll();

        FilenameSock = FilenameSock.left(FilenameSock.size() - 1);

        qInfo() << QString(FilenameSock);



        FILE* input = NULL;
        //system("D:\\minicap\\MiniCap.exe -capturedesktop -exit -save \"D:\\desktop.png\" -closeapp -exit ");
        input = fopen(FilenameSock.toStdString().c_str(), "rb");
        if (input == NULL)
        {
          qDebug("Не удалось открыть файл");
                  qInfo() << FilenameSock;
          exit(4);
        }
        rewind(input);

        fseek(input, 0L, SEEK_END);
        long long sizeFile = ftell(input);

        //long long sz = sizeFile + 4 + 4;
        //seek back:
        fseek(input, 0L, SEEK_SET);

        unsigned long long i=0;
        char *socksend = NULL;
       // char *socksend1 = NULL;
        socksend = (char*)malloc(sizeof(char) * (sizeFile));
        memset(socksend, 0, sizeFile);
       // socksend1 = (char*)malloc(sizeof(char) * (sizeFile));
        //socksend[0] = '2';
        char r[2];
        long long tt=0;
      //  while(1)
      //  {
             t = fread((void*)&socksend[0], sizeof(char), sizeFile, input);

            /* for(int n=0; n<100; n++) qDebug("%i", socksend[n]);
             if ((t + tt) < sizeFile)
             {
                 fseek(input, t+tt+1, 0);
                 socksend[tt + t] = 0;
                 for(int iu = tt; iu< t+tt; iu++)
                 {
                     socksend[iu] = socksend1[iu - tt];
                 }
                 tt += t;
             }
             else break;*/

       // }

       // if (feof(input))
       // qDebug("Ошибка чтения test.bin: неожиданный конец файла ");
       /* while(i < sizeFile)
        {
             r[0] = fgetc(input);

             if (i < sizeFile)
             socksend[i] = r[0];
             i++;
        }*/

        //for (QBluetoothSocket *socket : qAsConst(clientSockets)) socket->write(socksend);
        //long c = socket->write(socksend, sizeFile);
        i=0;
        char lin[9000];
        memset(lin, 0, 9000);
        lin[1] = 0;
        int u=0;

        QByteArray filesize;

        socket->write(filesize.setNum(sizeFile));

        qInfo() << sizeFile;
        while(i * 9000 < sizeFile)
        {
            u=0;
            while ((u < 9000) && ((u + i*9000) < sizeFile))
            {
                lin[u] = socksend[i* 9000 + u];
                u++;
            }
             Sleep(10);
             socket->write(lin, u);
             i++;
             qInfo() << i;
        }
       /* while (socket->canReadLine())
        {
            QByteArray line = socket->readLine().trimmed();
            emit messageReceived(socket->peerName(),
                                 QString::fromUtf8(line.constData(), line.length()));
        }*/
    }

}
//! [readSocket]
