#pragma once

#include <QTime>
#include <QUdpSocket>
#include <mutex>

extern std::mutex mutex;

struct AngleData {
    double xAngle, yAngle, zAngle;
    QTime time;

    bool operator<(const AngleData &d) {
        return time < d.time;
    }
};

extern QMap<QString, AngleData> map;

class UDPServer : public QObject {
    Q_OBJECT

    QUdpSocket *socket;

public:
    UDPServer();

private slots:
    void readPendingDatagram();
};
