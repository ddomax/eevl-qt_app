#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QDebug>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUIInitVal();
    board = QApplication::clipboard();
    board->setText("EasyExcel2USB Started!");
    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&captureTimer, SIGNAL(timeout()), this, SLOT(clipboard_changed()));
    spiDev = new SPI;
    netDev = new NetworkBusDev;
    //检测链接成功信号关联槽函数
    connect(netDev->tcp_clientSock,SIGNAL(connected()),this,SLOT(client_connect()));
    //检测掉线信号
    connect(netDev->tcp_clientSock,SIGNAL(disconnected()),this,SLOT(client_disconnect()));
}

MainWindow::~MainWindow()
{
    spiDev->deleteLater();
    netDev->deleteLater();
    delete ui;
}

void MainWindow::setupUIInitVal()
{
    ui->stopButton->setDisabled(true);
    ui->closeButton->setDisabled(true);
    ui->closeChannelButton->setDisabled(true);
    ui->ipLineEdit->setText("192.168.0.104");
    ui->portlineEdit->setText("7");
}

void MainWindow::Sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::client_connect()
{
    ui->connectButton->setDisabled(true);
    ui->closeButton->setEnabled(true);
}

void MainWindow::client_disconnect()
{
    ui->connectButton->setEnabled(true);
    ui->closeButton->setDisabled(true);
}

void MainWindow::on_connectButton_clicked()
{
    netDev->tcp_clientSock->connectToHost(ui->ipLineEdit->text(), ui->portlineEdit->text().toInt());
}

void MainWindow::on_closeButton_clicked()
{
    netDev->tcp_clientSock->close();
}

void MainWindow::clipboard_changed()
{

    QString content = board->text();
    QStringList list = content.split(":");
    if (list.at(0) == "readRegRequest" && list.length() == 2)
    {
        qDebug() << "readRegRequest";
        qDebug() << list.last();
//        QString addrHexString = list.last();
//        bool bStatus = false;
//        uint8_t addr = addrHexString.toUInt(&bStatus,16);
        uint8_t addr = list.last().toUInt();
        qDebug() << addr;
        uint8_t data[4];
        spiDev->readReg(addr, data, 2);
        uint16_t regVal = (data[0] << 8) + data[1];
        QString regValBinString;
        regValBinString.setNum(regVal, 2);
        QString paddedBinString = regValBinString.rightJustified(16, QLatin1Char('0'));
        qDebug() << paddedBinString;
        board->setText(paddedBinString);
    }
    else if (list.at(0) == "writeRegRequest" && list.length() == 3)
    {
        qDebug() << "writeRegRequest";
        qDebug() << "Address:" << list.at(1);
        qDebug() << "Content:" << list.at(2);
//        QString addrHexString = list.at(1);
        bool bStatus = false;
//        uint8_t addr = addrHexString.toUInt(&bStatus,16);
        uint8_t addr = list.at(1).toUInt();
        QString valueBinString = list.at(2);
        uint8_t data[4];
        data[0] = valueBinString.mid(0,8).toUInt(&bStatus,2);
        data[1] = valueBinString.mid(8,8).toUInt(&bStatus,2);
        qDebug() << "Content Value:" << data[0] << data[1];
        spiDev->writeReg(addr, data, 2);
        board->setText("Write Register Done!");
    }
    else if (list.at(0) == "AXIreadRegRequest" && list.length() == 2)
    {
        qDebug() << "AXIreadRegRequest";
        qDebug() << list.last();
//        QString addrHexString = list.last();
//        bool bStatus = false;
//        uint8_t addr = addrHexString.toUInt(&bStatus,16);
        uint32_t addr = list.last().toUInt();
        qDebug() << addr;
        uint32_t regVal;
        netDev->readReg(addr, &regVal);
        qDebug() << "regVal:" << regVal;
        QString regValBinString;
        regValBinString.setNum(regVal, 2);
        QString paddedBinString = regValBinString.rightJustified(32, QLatin1Char('0'));
        qDebug() << paddedBinString;
        board->setText(paddedBinString);
    }
    else if (list.at(0) == "AXIwriteRegRequest" && list.length() == 3)
    {
        qDebug() << "AXIwriteRegRequest";
        qDebug() << "Address:" << list.at(1);
        qDebug() << "Content:" << list.at(2);
//        QString addrHexString = list.at(1);
        bool bStatus = false;
//        uint8_t addr = addrHexString.toUInt(&bStatus,16);
        uint32_t addr = list.at(1).toUInt();
        QString valueBinString = list.at(2);
        uint32_t data = valueBinString.toUInt(&bStatus,2);
        qDebug() << "Content Value:" << data;
        netDev->writeReg(addr, data);
        board->setText("Write Register Done!");
    }

}

void MainWindow::on_startButton_clicked()
{
    board->setText("Started!");
    ui->startButton->setDisabled(true);
    ui->stopButton->setEnabled(true);
    captureTimer.start(50);
//    connect(board, SIGNAL(dataChanged()), this, SLOT(clipboard_changed())); 
}

void MainWindow::on_stopButton_clicked()
{
    board->setText("Stopped!");
    ui->startButton->setEnabled(true);
    ui->stopButton->setDisabled(true);
    captureTimer.stop();
//    disconnect(board, SIGNAL(dataChanged()), this, SLOT(clipboard_changed()));
}

void MainWindow::on_openChannelButton_clicked()
{
    spiDev->openChannel();
    ui->openChannelButton->setDisabled(true);
    ui->closeChannelButton->setEnabled(true);
}

void MainWindow::on_closeChannelButton_clicked()
{
    spiDev->closeChannel();
    ui->closeChannelButton->setDisabled(true);
    ui->openChannelButton->setEnabled(true);
}
