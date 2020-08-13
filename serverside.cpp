#include "serverside.h"

ServerSide::ServerSide(QObject *pwgt) : QObject(pwgt), m_nNextBlockSize(0)
{
    tcpServer = new QTcpServer(this);
}

QList<QTcpSocket *> ServerSide::getClients()
{
    return clients;
}

void ServerSide::newConnection()
{   /** Notice: this is a SLOT **/
    /*When a new connection is made ..*/

    /* (1) Create a new pointer and store the connection */
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();

    /* (2) Connects the clientConnection to special QTcpSocket signal / slot
     * My understanding: when this clientSocket gets discconected --> it will be send to delete list*/
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    /* (3) Connects the clientConnection to our custom signal / slot
     * My understanding: when this clientSocket gets new data --> we call our readClient function */
    connect(clientSocket, &QTcpSocket::readyRead, this, &ServerSide::readClient);
    /* (3) Connects the clientConnection to our custom signal / slot
     * My understanding: when this clientSocket gets discconected --> we call our gotDisconnection function */
    connect(clientSocket, &QTcpSocket::disconnected, this, &ServerSide::gotDisconnection);
    /* (4) Add the current connection to our clients list*/
    clients << clientSocket;
    /* (5) We call our sendToClient function and send him a reply to let him know we successfully connected*/
    sendToClient(clientSocket, "Reply: connection established");
}

void ServerSide::readClient()
{
    /* (1) Convert the sender [the signal that triggers this slot],to a QTcpSocket*/
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    /* (2) Read the data serialized into this client socket*/
    QDataStream in(clientSocket);
    /* (3) We start a infinite loop and we process the data from the clientSocket ..*/
    for(;;) //https://stackoverflow.com/a/3792210
    {
        /* (4) IF m_nNextBlockSize is zero ..*/
        if(!m_nNextBlockSize)
        {
            /* (5) IF the number of available bytes is less than sizeof quint16 we break the for loop*/
            if(clientSocket->bytesAvailable() < sizeof(quint16)) {break;}
            /* (6) we transfer bytes from the socket into m_nNextblockSize variable.
             * (Not quite sure about this yet)When its limit is reached (16bits?), it continues
             */
            in >> m_nNextBlockSize;
        }
        if(clientSocket->bytesAvailable() < m_nNextBlockSize) {break;}
        /* (7) convert the bytes into a string*/
        QString str;
        in >> str;
        /* (8) Emit our message*/
        emit gotNewMessage(str);
        /* (9) set the buffer to 0. We empty it */
        m_nNextBlockSize = 0;

        /* (10) We try to send a message to the client that we recived its package.*/
        if(sendToClient(clientSocket, QString("Reply: recieved: [%1]").arg(str)) == -1)
        {
            /* (11) IF it fails for any reason we log a message*/
            qDebug() << "Some error occured";
        }
    }
}

void ServerSide::gotDisconnection()
{
    /* (1) Remove the clientSocket that triggers this function from our list*/
    clients.removeAt(clients.indexOf((QTcpSocket*)sender()));
    /* (2) Trigger somebodyDisconnect function*/
    emit smbDisconnected();
}

qint64 ServerSide::sendToClient(QTcpSocket* socket, const QString &str)
{
    /* (1) Create a Byte Array to store our bytes */
    QByteArray arrBlock;
    /* (2) Create a datastream based on our arrBlock with a WriteOnly scope */
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    /* (3) Transform our string into bytes and add it to the out stream */
    out << quint16(0) << str;
    /* (4) ..Not sure what this does yet but looks cool */
    out.device()->seek(0);
    /* (5) ..Not sure */
    out <<quint16(arrBlock.size() - sizeof(quint16));
    /* (6) Write the arrBlock to the socket, which in turn returns a value:
     *  -1 if not succesful
     *  other number representing the number of bytes written*/
    return socket->write(arrBlock);
}
