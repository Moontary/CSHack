#include "udpserver.h"

#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QTime>
#include <iostream>

QMap<QString, AngleData> map;
std::mutex mutex;

UDPServer::UDPServer() {
    socket = new QUdpSocket(this);
    socket->bind(43567);

    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();

    for (QHostAddress addr : addresses)
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && addr != QHostAddress(QHostAddress::LocalHost))
            std::cout << "IP: " << addr.toString().toStdString() << std::endl;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagram()));
}

void UDPServer::readPendingDatagram() {
    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        qDebug() << datagram.data();

        QString str = datagram.data();
        QStringList list = str.split(' ');

        mutex.lock();

        map[list[0]].xAngle = list.at(1).toDouble();
        map[list[0]].yAngle = list.at(2).toDouble();
        map[list[0]].zAngle = list.at(3).toDouble();

        map[list[0]].yAngle = std::max(-90.0, map[list[0]].yAngle);
        map[list[0]].yAngle = std::min(90.0, map[list[0]].yAngle);

        map[list[0]].time = QTime::currentTime();

        mutex.unlock();
    }
}
