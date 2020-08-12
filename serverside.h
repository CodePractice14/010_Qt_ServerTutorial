#ifndef SERVERSIDE_H
#define SERVERSIDE_H

#include <QObject>

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QList>

class ServerSide : public QObject
{
    Q_OBJECT
public:
    ServerSide(QObject *pwgt);
    QTcpServer *tcpServer;
    QList<QTcpSocket *> getClients();

public slots:
    virtual void newConnection();
    void readClient();
    void gotDisconnection();
    qint64 sendToClient(QTcpSocket *socket, const QString &str);

signals:
    void gotNewMessage(QString msg);
    void smbDisconnected();

private:
    quint16 m_nNextBlockSize;
    QList<QTcpSocket *> clients;
};

#endif // SERVERSIDE_H
