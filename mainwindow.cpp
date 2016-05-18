#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QLabel>
#include <QMessageBox>
#include <algorithm>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    init();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){
    //GUI
    gl = new gpsDisplay(ui->tab);
    al = new accDisplay(ui->tab);
    //BASE TIME
    base_timer = new QTimer(this);
    base_timer_time = 1000;
    base_time = 0;
    connect(base_timer,SIGNAL(timeout()),this,SLOT(base_time_slot()));
    base_timer->start(base_timer_time);

    //GPS related
    GPS_button_status = false;
    GPS_status = false;
    ui->GPS_begin_button->setEnabled(true);
    ui->GPS_stop_button->setEnabled(false);
    GPS_serial = new QSerialPort(this);
    GPS_serial->setBaudRate(QSerialPort::Baud9600);
    GPS_serial->setDataBits(QSerialPort::Data8);
    GPS_serial->setParity(QSerialPort::NoParity);
    GPS_serial->setStopBits(QSerialPort::OneStop);
    GPS_serial->setFlowControl(QSerialPort::NoFlowControl);
    GPS_serial->setPortName("/dev/ttyUSB0");
    GPS_timer = new QTimer(this);
    GPS_timer_time = 500;
    connect(GPS_timer, SIGNAL(timeout()), this, SLOT(GPS_slot()));
    connect(GPS_serial, SIGNAL(readyRead()), this, SLOT(serial_getData()));

    //ACC related
    ACC_button_status = false;
    GPS_status = false;
    ui->ACC_begin_button->setEnabled(true);
    ui->ACC_stop_button->setEnabled(false);
    ACC_serial = new QSerialPort(this);
    ACC_serial->setBaudRate(QSerialPort::Baud115200);
    ACC_serial->setDataBits(QSerialPort::Data8);
    ACC_serial->setParity(QSerialPort::NoParity);
    ACC_serial->setStopBits(QSerialPort::OneStop);
    ACC_serial->setFlowControl(QSerialPort::NoFlowControl);
    ACC_serial->setPortName("/dev/ttyUSB1");

}
void MainWindow::base_time_slot(){
    base_time++;
    ui->base_time_label->setNum(base_time);
}

void MainWindow::on_GPS_begin_button_clicked()
{
    if(!GPS_serial->open(QIODevice::ReadOnly)){
        QMessageBox::information(this,tr("USART"),"Can not open the GPS usart!");
    }else{
        GPS_button_status = true;
        gl->GPS_status = GPS_button_status;
        gl->update();
        ui->GPS_begin_button->setEnabled(false);
        ui->GPS_stop_button->setEnabled(true);
        GPS_timer->start(GPS_timer_time);
    }
}

void MainWindow::on_GPS_stop_button_clicked()
{
    GPS_button_status = false;
    gl->GPS_status = GPS_button_status;
    gl->update();
    ui->GPS_begin_button->setEnabled(true);
    ui->GPS_stop_button->setEnabled(false);
    GPS_timer->stop();
    GPS_serial->close();
}
void MainWindow::serial_getData(){
    GPS_timer->start(GPS_timer_time); // 开始接收GPS信号之后延时500ms
}

void MainWindow::GPS_slot(){ //读取串口缓存并处理
    GPS_timer->stop();
    GPS_rxdata.clear();
    GPS_rxdata = (QString)GPS_serial->readAll();
    gpsInfo.infoProcess(GPS_rxdata);
    if(gpsInfo.effective==false){
        ui->GPS_latitude->setText("NaN");
        ui->GPS_longitude->setText("NaN");
        ui->GPS_speed->setText("NaN");
        ui->GPS_satellite_num->setText("NaN");
        ui->GPS_UTCdate->setText("NaN");
        ui->GPS_UTCtime->setText("NaN");
    }
    else{
        ui->GPS_latitude->setText(QString(gpsInfo.gprmc.NorS)+"  "+QString(gpsInfo.gprmc.Latitude));
        ui->GPS_longitude->setText(QString(gpsInfo.gprmc.EorW)+"  "+QString(gpsInfo.gprmc.Longitude));
        ui->GPS_speed->setText(QString(gpsInfo.gprmc.Speed));
        ui->GPS_satellite_num->setText(QString(gpsInfo.gpgga.Quantity));
        ui->GPS_UTCdate->setText(QString(gpsInfo.gprmc.UTCdate));
        ui->GPS_UTCtime->setText(QString(gpsInfo.gprmc.UTCtime));
        ui->GPS_accuracy->setText(QString(gpsInfo.gpgga.HDOP));
    }
}

void MainWindow::on_ACC_begin_button_clicked()
{
    if(!ACC_serial->open(QIODevice::ReadOnly)){
        QMessageBox::information(this,tr("UART"),"Can not open the ACC usart!");
    }else{
        ACC_button_status = true;
        al->ACC_status = ACC_button_status;
        al->update();
        ui->ACC_begin_button->setEnabled(false);
        ui->ACC_stop_button->setEnabled(true);
        //GPS_timer->start(GPS_timer_time);
    }
}

void MainWindow::on_ACC_stop_button_clicked()
{
    ACC_button_status = false;
    al->ACC_status = ACC_button_status;
    al->update();
    ui->ACC_begin_button->setEnabled(true);
    ui->ACC_stop_button->setEnabled(false);
    ACC_serial->close();
}
