#include "mainserverwindow.h"
#include "ui_mainserverwindow.h"

MainServerWindow::MainServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainServerWindow)
{
    ui->setupUi(this);

    /* (1) Create a new instance of our ServerSide object */
    server = new ServerSide(this);
    /* (2) Connect signal gotNewMessage from our server, to slot gotNewMessage from this MainServerWindow */
    connect(server, &ServerSide::gotNewMessage, this, &MainServerWindow::gotNewMessage);
    /* (3) Connect signal newConnection from the tcpServer object on server, to slot smbConnectedToServer from this MainServerWindow */
    connect(server->tcpServer, &QTcpServer::newConnection, this, &MainServerWindow::smbConnectedToServer);
    /* (4) Connect signal smbDisconnected from our server, to slot smbDisconnectedFromServer from this MainServerWindow */
    connect(server, &ServerSide::smbDisconnected, this, &MainServerWindow:: smbDisconnectedFromServer);

    /* (5) Connect our buttons to their proper usage
     * ..I chose to do this in code rather than using the Designer so its easier to follow*/
    connect(ui->btnStartServer, &QPushButton::clicked, this, &MainServerWindow::on_pushButton_startServer_clicked);
    connect(ui->btnStopServer, &QPushButton::clicked, this, &MainServerWindow::on_pushButton_stopServer_clicked);
    connect(ui->btnTestConnection, &QPushButton::clicked, this, &MainServerWindow::on_pushButton_testConnection_clicked);
}

MainServerWindow::~MainServerWindow()
{
    /* (1) When this window closes DELETE our server!
     * If we don't do this we will suffer a memory leak..
     * ..that means the server will remain open in memory*/
    delete server;
    delete ui;
}

void MainServerWindow::on_pushButton_startServer_clicked()
{
    /* (1) Attempt to open the tcpServer from our server and make it listen on the given address/port.. */
    if(!server->tcpServer->listen(QHostAddress::Any, 6547))
    {
        /* (2) IF it can't connect for any reason display an error message and exit this function */
        ui->txtServerLog->append(tr("<font color=\"red\"><b>Error!</b> The port is taken by some other service.</font>"));
        return;
    }
    /* (3) IF it connects, we connect the tcpServer singal to our newConnection Slot from the server
     * ..which will add it to our open connections list*/
    connect(server->tcpServer, &QTcpServer::newConnection, server, &ServerSide::newConnection);
    /* (4) Display a success message */
    ui->txtServerLog->append(tr("<font color=\"green\"><b>Server started</b>, port is opened.</font>"));
}

void MainServerWindow::on_pushButton_stopServer_clicked()
{
    /* (1) IF the server is listening...which implies it is open */
    if(server->tcpServer->isListening())
    {
        /* (2) Send the discconect signal to the newConnection slot running on the server
         * ..This means that it closes/disloves the slot that was listening for any new connections */
        disconnect(server->tcpServer, &QTcpServer::newConnection, server, &ServerSide::newConnection);
        /* (3) Get a list of all connected clients currently connected to the server */
        QList<QTcpSocket *> clients = server->getClients();
        /* (4) For each of those clients send one last message :0 ..they'll know what it means */
        for(int i=0; i<clients.count(); i++)
        {
            server->sendToClient(clients.at(i), "0");
        }
        /* (5) Close the tcpServer */
        server->tcpServer->close();
        /* (6) Display a message */
        ui->txtServerLog->append(tr("<b>Server stopped</b>, post is closed"));
    }
    else
    {   /* (7) IF the server was not listening, means it wasn't running so we display a message */
        ui->txtServerLog->append(tr("<b>Error!</b> Server was not running"));
    }
}

void MainServerWindow::on_pushButton_testConnection_clicked()
{
    /* (1) IF our tcpServer is listening.. */
    if(server->tcpServer->isListening())
    {
        /* (2) display a message including the number of connected clients*/
        ui->txtServerLog->append(
                    QString("%1 %2")
                    .arg("Server is listening, number of connected clients:")
                    .arg(QString::number(server->getClients().count()))
                    );
    }
    else
    {
        /* (3) IF its not listening display a message including the number of connected clients */
        ui->txtServerLog->append(
                    QString("%1 %2")
                    .arg("Server is not listening, number of connected clients:")
                    .arg(QString::number(server->getClients().count()))
                    );
    }
}

void MainServerWindow::smbConnectedToServer()
{
    ui->txtServerLog->append(tr("Somebody has connected"));
}

void MainServerWindow::smbDisconnectedFromServer()
{
    ui->txtServerLog->append(tr("Somebody has disconnected"));
}
void MainServerWindow::gotNewMessage(QString msg)
{
    ui->txtServerLog->append(QString("New Message: %1").arg(msg));
}




















































