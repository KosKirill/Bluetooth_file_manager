#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <string.h>
#include "chatclient.h"
#include "chatserver.h"

#include <QMainWindow>
#include <QFile>
#include <QtWidgets/qdialog.h>

#include <QtBluetooth/qbluetoothhostinfo.h>

#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>



#include <QDialog>
#include <QPointer>

#include <qbluetoothuuid.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <QBluetoothSocket>

QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QTableWidgetItem)
QT_FORWARD_DECLARE_CLASS(QFile)





QT_FORWARD_DECLARE_CLASS(QBluetoothServiceDiscoveryAgent)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)


namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QBluetoothServiceInfo service() const;
    char* name_file;
signals:
    void sendMessage(const QString &message);

private slots:
   // void connectClicked();
   // void sendClicked();

    void showMessage(const QString &sender, const QString &message);

    void clientConnected(const QString &name);
   // void clientDisconnected(const QString &name);
   // void clientDisconnected();
  //  void connected(const QString &name);
  //  void reactOnSocketError(const QString &error);


    void on_pushButton_clicked();
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);
    void update_bar(int num);
    void write(QByteArray src, bool del);
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void connect_to();
    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_lineEdit_editingFinished();

    void on_pushButton_3_toggled(bool checked);


    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QByteArray received_sh;
    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QBluetoothServiceInfo m_service;
    QMap<QListWidgetItem *, QBluetoothServiceInfo> m_discoveredServices;

    ChatServer *server;
    QList<ChatClient *> clients;
    QList<QBluetoothHostInfo> localAdapters;

    QString localName;
};

#endif // MAINWINDOW_H


