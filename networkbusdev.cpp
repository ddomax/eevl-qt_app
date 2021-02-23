#include "networkbusdev.h"
#include "QHostAddress"

NetworkBusDev::NetworkBusDev(QObject *parent) : QObject(parent)
{
    tcp_clientSock = new QTcpSocket;
}

NetworkBusDev::~NetworkBusDev()
{
    tcp_clientSock->deleteLater();
}

int NetworkBusDev::writeReg(uint32_t address, uint32_t data)
{
    QString writeCMD = "writeRequest:" + QString::number(address, 16) +
            ":" + QString::number(data, 16) + '\0';
    qDebug() << "writeCMD" << writeCMD;
    tcp_clientSock->write(writeCMD.toLocal8Bit());
    tcp_clientSock->waitForReadyRead(10000);
    QByteArray readbackVal = tcp_clientSock->readAll();
    while(readbackVal.isEmpty())
    {
        readbackVal = tcp_clientSock->readAll();
    }
    qDebug() << "readbackVal:" << readbackVal;
    return 0;
}

int NetworkBusDev::readReg(uint32_t address, uint32_t *data)
{
    tcp_clientSock->readAll(); // Clear buffer
    QString readCMD = QString("readRequest:") + QString::number(address,16) + '\0';
    qDebug() << readCMD;
    tcp_clientSock->write(readCMD.toLocal8Bit());
    tcp_clientSock->waitForReadyRead(10000);
    QByteArray readbackVal = tcp_clientSock->readAll();
    while(readbackVal.isEmpty())
    {
        readbackVal = tcp_clientSock->readAll();
    }
    qDebug() << "readbackVal:" << readbackVal;
    bool bStatus = false;
    *data = readbackVal.toUInt(&bStatus, 16);
//    *data = 99;
    return 0;
}
