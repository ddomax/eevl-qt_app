#ifndef NETWORKBUSDEV_H
#define NETWORKBUSDEV_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class NetworkBusDev : public QObject
{
    Q_OBJECT
public:
    explicit NetworkBusDev(QObject *parent = nullptr);
    ~NetworkBusDev();
    QTcpSocket *tcp_clientSock;
    int writeReg(uint32_t address, uint32_t data);
    int readReg(uint32_t address, uint32_t *data);

private:
//    QTcpSocket *tcp_clientSock;

signals:

};

#endif // NETWORKBUSDEV_H
