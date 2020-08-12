#ifndef MAINSERVERWINDOW_H
#define MAINSERVERWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "serverside.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainServerWindow; }
QT_END_NAMESPACE

class MainServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainServerWindow(QWidget *parent = nullptr);
    ~MainServerWindow();

private slots:
    void on_pushButton_stopServer_clicked();
    void on_pushButton_startServer_clicked();
    void on_pushButton_testConnection_clicked();
    void smbConnectedToServer();
    void smbDisconnectedFromServer();
    void gotNewMessage(QString msg);

private:
    Ui::MainServerWindow *ui;
    ServerSide *server;
};
#endif // MAINSERVERWINDOW_H
