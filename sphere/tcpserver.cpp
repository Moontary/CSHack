#include "tcpserver.h"

#include <QNetworkInterface>
#include <QTcpSocket>
#include <iostream>

TCPServer::TCPServer() {
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 43567);

    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for (QHostAddress addr : addresses)
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress(QHostAddress::LocalHost))
            std::cout << "IP: " << addr.toString().toStdString() << std::endl;

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void TCPServer::newConnection() {
    QTcpSocket *clientConnection = server->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(readPendingDatagram()));
}

void TCPServer::readPendingDatagram() {
    QTcpSocket *socket = reinterpret_cast<QTcpSocket *>(sender());
    qDebug() << socket->readAll();
}
