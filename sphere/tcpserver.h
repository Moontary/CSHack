#pragma once

#include <QTcpServer>

class TCPServer : public QObject {
    Q_OBJECT

    QTcpServer *server;

public:
    TCPServer();

private slots:
    void newConnection();
    void readPendingDatagram();
};
