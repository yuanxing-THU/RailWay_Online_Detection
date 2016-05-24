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
    ACC_serial->setPortName("/dev/ttyUSB0");

    //ACC PLOT test
    ui->ACC_plot->legend->setVisible(true);
    ui->ACC_plot->addGraph();
    ui->ACC_plot->addGraph();
    ui->ACC_plot->addGraph();
    ui->ACC_plot->graph(0)->setPen(QPen(Qt::blue));
    ui->ACC_plot->graph(0)->setName("x");
    ui->ACC_plot->graph(1)->setPen(QPen(Qt::red));
    ui->ACC_plot->graph(1)->setName("y");
    ui->ACC_plot->graph(2)->setPen(QPen(Qt::green));
    ui->ACC_plot->graph(2)->setName("z");
    ui->ACC_plot->xAxis->setLabel("t");
    ui->ACC_plot->yAxis->setLabel("xyz");
    ui->ACC_plot->xAxis->setRange(0, 100);
    ui->ACC_plot->yAxis->setRange(-512, 512);
//    QVector<double> x(101), y(101); // initialize with entries 0..100
//    for (int i=0; i<101; ++i)
//    {
//      x[i] = i/50.0 - 1; // x goes from -1 to 1
//      y[i] = x[i]*x[i]; // let's plot a quadratic function
//    }
//    ui->ACC_plot->graph(0)->setData(x, y);
//    ui->ACC_plot->xAxis->setLabel("x");
//    ui->ACC_plot->yAxis->setLabel("y");
//    ui->ACC_plot->xAxis->setRange(0, 1000);
//    ui->ACC_plot->yAxis->setRange(-512, 512);
//    ui->ACC_plot->replot();
}
void MainWindow::base_time_slot(){
    base_time++;

    ui->base_time_label->setNum(base_time);
    ACC_rxdata.clear();
    ACC_rxdata = ACC_serial->readAll();

    QVector <double> x,y,z,t;
    int k=0;

    if(ACC_button_status){
        //ACC_rxdata processing
        int l=ACC_rxdata.size();
        int i=0;
        signed short temp;
        while(i<l){
            if(ACC_rxdata[i]=='\n'){
                if(ACC_rxdata[i+1]=='\n'){
                    i+=2;
                    break;
                }
            }
            i++;
        }
        if(i<l){
            while(i+6<l){
                if(ACC_rxdata[i+6]=='\n'&&ACC_rxdata[i+7]=='\n'){
                    k++;
                    t.push_back(k);
                    temp = (ACC_rxdata[i]<<8)+ACC_rxdata[i+1];
//                    if(temp&0xF000){
//                        temp=~temp+1;
//                    }
                    x.push_back(temp);
                    temp = (ACC_rxdata[i+2]<<8)+ACC_rxdata[i+3];
//                    if(temp&0xF000){
//                        temp=~temp+1;
//                    }
                    y.push_back(temp);
                    temp = (ACC_rxdata[i+4]<<8)+ACC_rxdata[i+5];
//                    if(temp&0xF000){
//                        temp=~temp+1;
//                    }
                    z.push_back(temp);

                }
                i+=8;
            }
        }

        ui->ACC_plot->graph(0)->setData(t, x);
        ui->ACC_plot->graph(1)->setData(t, y);
        ui->ACC_plot->graph(2)->setData(t, z);
        ui->ACC_plot->replot();
    }
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
