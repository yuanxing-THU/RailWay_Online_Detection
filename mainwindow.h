#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "gpsDisplay.h"
#include "accDisplay.h"
#include "GPS_info.h"
#include "storeDisplay.h"
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <qstring.h>
#include<QDateTime>
#include<QFile>
#include<QDir>
#include<QTextStream>
#include<QCloseEvent>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:

    Ui::MainWindow *ui;
//variable
  //GPS
    gpsDisplay *gl; //状态显示label
    bool GPS_button_status;
    bool GPS_status;
    int GPS_fd; //串口设备文件描述
    QSerialPort *GPS_serial;
    QSerialPortInfo *GPS_serialinfo;
    int GPS_timer_time;
    QTimer *GPS_timer;
    GPS_info gpsInfo;
    QString GPS_rxdata;
  //ACC
    accDisplay *al;
    bool ACC_button_status;
    bool ACC_status;
    int ACC_fd;
    QSerialPort *ACC_serial;
    QSerialPortInfo *ACC_serialinfo;
    QByteArray ACC_rxdata;

  //BASE TIME
    QTimer *base_timer;
    int base_timer_time;  //time interval
    int base_time;  // unique ID

    QDateTime nowtime;
    QDir *myDir;
    QString dirname;
    QFile *GPSfile;
    QFile *ACCfile;
    QTextStream *GPSout;
    QTextStream *ACCout;
    bool save_status;
    //function
    void init();

    int file_num;

    storeDisplay *sl;

private slots:
    void on_ACC_stop_button_clicked();
    void on_ACC_begin_button_clicked();
    void on_GPS_stop_button_clicked();
    void on_GPS_begin_button_clicked();
    void GPS_slot();
    void serial_getData();
    void base_time_slot();
    void on_SAVE_button_clicked();
    void on_save_stop_button_clicked();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
