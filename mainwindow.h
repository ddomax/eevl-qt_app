#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "spi.h"
#include "networkbusdev.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QClipboard *board;
    void Sleep(int msec);
    void setupUIInitVal();
    QTimer captureTimer;
    SPI *spiDev;
    NetworkBusDev *netDev;

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_connectButton_clicked();
    void on_closeButton_clicked();
    void on_openChannelButton_clicked();
    void on_closeChannelButton_clicked();
    void clipboard_changed();
    void client_connect();
    void client_disconnect();
};
#endif // MAINWINDOW_H
