#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QLabel>
#include <QMessageBox>
#include <algorithm>
#include <QString>
#include "myWavelet.h"
#include "FFT.h"
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
    sl = new storeDisplay(ui->tab);
    //BASE TIME
    base_timer = new QTimer(this);
    base_time = 0;
    base_timer_time = 1000;
    connect(base_timer,SIGNAL(timeout()),this,SLOT(base_time_slot()));
    base_timer->start(base_timer_time);

    //GPS related
    GPS_button_status = false;
    GPS_status = false;
    ui->GPS_begin_button->setEnabled(true);
    ui->GPS_stop_button->setEnabled(false);
    GPS_serial = new QSerialPort(this);
    GPS_serial->setBaudRate(QSerialPort::Baud38400);
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

    int L_plot = 800*base_timer_time/1000;

    QPen pen;
    pen.setWidth(5);
    pen.setColor(Qt::blue);
    //tab3 ACC PLOT test
    ui->ACC_y_plot->addGraph();
    ui->ACC_y_plot->graph(0)->setPen(pen);
    ui->ACC_y_plot->xAxis->setLabel("t");
    ui->ACC_y_plot->yAxis->setLabel("Y");
    ui->ACC_y_plot->xAxis->setRange(0, L_plot);
    ui->ACC_y_plot->yAxis->setRange(-1024, 1024);

    ui->ACC_z_plot->addGraph();
    ui->ACC_z_plot->graph(0)->setPen(pen);
    ui->ACC_z_plot->xAxis->setLabel("t");
    ui->ACC_z_plot->yAxis->setLabel("Z");
    ui->ACC_z_plot->xAxis->setRange(0, L_plot);
    ui->ACC_z_plot->yAxis->setRange(-1024, 1024);

    //tab4 wavelet

    ui->A5->addGraph();
    ui->A5->graph(0)->setPen(pen);
    ui->A5->xAxis->setRange(0, L_plot);
    ui->A5->yAxis->setRange(-512, 512);

    ui->D5->addGraph();
    ui->D5->graph(0)->setPen(pen);
    ui->D5->xAxis->setRange(0, L_plot);
    ui->D5->yAxis->setRange(-200, 200);

    ui->D4->addGraph();
    ui->D4->graph(0)->setPen(pen);
    ui->D4->xAxis->setRange(0, L_plot);
    ui->D4->yAxis->setRange(-200, 200);

    ui->D3->addGraph();
    ui->D3->graph(0)->setPen(pen);
    ui->D3->xAxis->setRange(0, L_plot);
    ui->D3->yAxis->setRange(-200, 200);

    ui->D2->addGraph();
    ui->D2->graph(0)->setPen(pen);
    ui->D2->xAxis->setRange(0, L_plot);
    ui->D2->yAxis->setRange(-200, 200);

    ui->D1->addGraph();
    ui->D1->graph(0)->setPen(pen);
    ui->D1->xAxis->setRange(0, L_plot);
    ui->D1->yAxis->setRange(-200, 200);

    //tab_5 FFT
    ui->fft->addGraph();
    ui->fft->graph(0)->setPen(pen);
    ui->fft->xAxis->setRange(0, 400);
    ui->fft->yAxis->setRange(-200, 200);

    //save file
    save_status = false;
    ui->save_stop_button->setEnabled(false);
    file_num = 1;
}
void MainWindow::base_time_slot(){
    base_time++;
    nowtime = QDateTime::currentDateTime();
    ui->base_time_label->setText(nowtime.toString("yyyy-MM-dd hh:mm:ss"));
    ACC_rxdata.clear();
    ACC_rxdata = ACC_serial->readAll();

    if(save_status==true&&base_time==3600){
        save_status=false;
        base_time=0;
        ACCfile->close();
        GPSfile->close();
        file_num++;
        ACCfile = new QFile(dirname+"/ACC"+QString::number(file_num,10)+".txt");
        GPSfile = new QFile(dirname+"/GPS"+QString::number(file_num,10)+".txt");
        if(!GPSfile->open(QIODevice::WriteOnly | QIODevice::Text)||
                !ACCfile->open(QIODevice::WriteOnly | QIODevice::Text))
            QMessageBox::information(this,tr("FILE"),"Can not open gps or acc file!");

        ACCout = new QTextStream(ACCfile);
        GPSout = new QTextStream(GPSfile);

        save_status=true;
    }

    if(save_status == true)
        *ACCout <<"basetime:"<<base_time<<"\n";
    if(ACC_button_status){
        QVector <double> y,z,t; //y,z
        const int Fs=800;
        const int NFFT=1024;
        const int interN=10;
        complex <double> cz[NFFT];
        complex <double> cy[NFFT];
        QVector<double> vz,vy,t_fft;
        for(int i=0;i<NFFT;i++) {
            cz[i]=0;
            cy[i]=0;
        }
        int k=0;


        //ACC_rxdata processing
        int l=ACC_rxdata.size();
        int i=0;
        signed short tempy,tempz;
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
            while(i+4<l){
                if(ACC_rxdata[i+4]=='\n'&&ACC_rxdata[i+5]=='\n'){
                    k++;
                    t.push_back(k);
                    tempz = (ACC_rxdata[i]<<8)+ACC_rxdata[i+1];
                    z.push_back(tempz);
                    tempy = (ACC_rxdata[i+2]<<8)+ACC_rxdata[i+3];
                    y.push_back(tempy);
                    if(save_status == true)
                        *ACCout <<tempy<<" "<<tempz<<"\n";
                }
                i+=6;
            }
        }


//TQI
//        QFile filey("test_y.txt");
//        QFile filez("test_z.txt");
//        QTextStream ts_y(&filey);
//        QTextStream ts_z(&filez);
//        QString linestr;
//        //y
//        z.clear();y.clear();
//        if(!filey.open(QIODevice::ReadOnly | QIODevice::Text))
//        {
//            cout << "Open failed." << endl;
//        }
//        while(!ts_y.atEnd())
//        {
//            linestr = ts_y.readLine();
//            y.push_back(256*linestr.toDouble());
//        }
//        filey.close();
//        cout<<y.size()<<endl;
//        //z
//        if(!filez.open(QIODevice::ReadOnly | QIODevice::Text))
//        {
//            cout << "Open failed." << endl;
//        }
//        i=0;
//        while(!ts_z.atEnd())
//        {
//            t.push_back(i);
//            i++;
//            linestr = ts_z.readLine();
//            z.push_back(256*linestr.toDouble());
//        }
//        filez.close();
//        cout<<z.size()<<endl;
        double L=z.size();

        ui->ACC_z_plot->graph(0)->setData(t, z);
        ui->ACC_z_plot->replot();

        ui->ACC_y_plot->graph(0)->setData(t, y);
        ui->ACC_y_plot->replot();


        if(L>base_timer_time/1000*Fs-300){
            // mean zero
            double zsum=0,ysum=0;
            i=0;
            for(i=0;i<z.size();i++){
                zsum += z[i];
                ysum += y[i];
            }
            double zarg=zsum/i,yarg=ysum/i;
            for(i=0;i<z.size();i++){
                z[i] = z[i]-zarg;
                y[i] = y[i]-yarg;
                cz[i] = z[i];
                cy[i] = y[i];
            }
            //FFT

            complex<double> FD[NFFT];

            FFT(cz,FD,interN);
            for(int i=0;i<NFFT/2+1;i++)
            {
                vz.push_back(2*abs(FD[i]/L));
                t_fft.push_back(i*Fs/2/(NFFT/2+1));
            }
            ui->fft->graph(0)->setData(t_fft,vz);
            ui->fft->replot();
            FFT(cy,FD,interN);
            for(int i=0;i<NFFT/2+1;i++)
            {
                vy.push_back(2*abs(FD[i]/L));
            }

            //feature
            double feature[24];
            for(int i=0;i<24;i++){
                feature[i]=0;
            }
            double z2sum=0;
            double y2sum=0;
            double z3sum=0;
            double y3sum=0;
            double z4sum=0;
            double y4sum=0;
            double zmax=z[0];
            double zmin=z[0];
            double ymax=y[0];
            double ymin=y[0];
            zsum = 0;
            ysum = 0;

            //time domain
            for(int i=0;i<z.size();i++){
                zsum += abs(z[i]);
                ysum += abs(y[i]);
                z2sum += z[i]*z[i];
                y2sum += y[i]*y[i];
                z3sum += abs(z[i]*z[i]*z[i]);
                y3sum += abs(y[i]*y[i]*y[i]);
                z4sum += z[i]*z[i]*z[i]*z[i];
                y4sum += y[i]*y[i]*y[i]*y[i];
                if(z[i]>zmax) zmax=z[i];
                if(z[i]<zmin) zmin=z[i];
                if(y[i]>ymax) ymax=y[i];
                if(y[i]<ymin) ymin=y[i];
            }
            feature[0]=zsum/z.size();
            feature[1]=ysum/y.size();
            feature[2]=zmax-zmin;
            feature[3]=ymax-ymin;
            feature[4]=sqrt(z2sum/z.size());
            feature[5]=sqrt(y2sum/y.size());
            feature[6]=z2sum/z.size();
            feature[7]=y2sum/y.size();
            feature[8]=z3sum/z.size();
            feature[9]=y3sum/y.size();
            feature[10]=z4sum/z.size();
            feature[11]=y4sum/y.size();
            //frequency domain
            zsum = 0;
            ysum = 0;
            z2sum=0;
            y2sum=0;
            double z2asum=0;
            double y2asum=0;
            z3sum=0;
            y3sum=0;
            z4sum=0;
            y4sum=0;
            zmax=vz[0];
            zmin=vz[0];
            ymax=vy[0];
            ymin=vy[0];

            for(int i=0;i<vz.size();i++){
                zsum += vz[i];
                ysum += vy[i];
            }
            double za=zsum/vz.size(),ya=ysum/vy.size();
            zsum = 0;
            ysum = 0;
            for(int i=0;i<vz.size();i++){
                zsum += abs(vz[i]);
                ysum += abs(vy[i]);
                z2sum += vz[i]*vz[i];
                y2sum += vy[i]*vy[i];
                z2asum += (vz[i]-za)*(vz[i]-za);
                y2asum += (vy[i]-ya)*(vy[i]-ya);
                z3sum += vz[i]*vz[i]*vz[i];
                y3sum += vy[i]*vy[i]*vy[i];
                z4sum += vz[i]*vz[i]*vz[i]*vz[i];
                y4sum += vy[i]*vy[i]*vy[i]*vy[i];
                if(vz[i]>zmax) zmax=vz[i];
                if(vz[i]<zmin) zmin=vz[i];
                if(vy[i]>ymax) ymax=vy[i];
                if(vy[i]<ymin) ymin=vy[i];
            }
            feature[12]=zsum/vz.size();
            feature[13]=ysum/vy.size();
            feature[14]=zmax-zmin;
            feature[15]=ymax-ymin;
            feature[16]=sqrt(z2sum/vz.size());
            feature[17]=sqrt(y2sum/vy.size());
            feature[18]=z2asum/vz.size();
            feature[19]=y2asum/vy.size();
            feature[20]=z3sum/vz.size();
            feature[21]=y3sum/vy.size();
            feature[22]=z4sum/vz.size();
            feature[23]=y4sum/vy.size();
            ui->TQI_label->setText(QString::number(feature[0]*0.3,'g',6));

     //wavelet
            std::vector <double> C,a5,d5,d4,d3,d2,d1,Z=z.toStdVector();
            std::vector <int> L;
            mywaveDec(Z,5,C,L);
            mywrcoef('a',C,L,5,a5);
            mywrcoef('d',C,L,5,d5);
            mywrcoef('d',C,L,4,d4);
            mywrcoef('d',C,L,3,d3);
            mywrcoef('d',C,L,2,d2);
            mywrcoef('d',C,L,1,d1);

            ui->A5->graph(0)->setData(t, QVector<double>::fromStdVector(a5));
            ui->A5->replot();

            ui->D5->graph(0)->setData(t, QVector<double>::fromStdVector(d5));
            ui->D5->replot();

            ui->D4->graph(0)->setData(t, QVector<double>::fromStdVector(d4));
            ui->D4->replot();

            ui->D3->graph(0)->setData(t, QVector<double>::fromStdVector(d3));
            ui->D3->replot();

            ui->D2->graph(0)->setData(t, QVector<double>::fromStdVector(d2));
            ui->D2->replot();

            ui->D1->graph(0)->setData(t, QVector<double>::fromStdVector(d1));
            ui->D1->replot();
            vector <double> dh,corr_f;
            for(int i=0;i<d5.size();i++){
                dh.push_back(abs(d5[i]+d4[i]+d3[i]+d2[i]+d1[i]));
            }
            double dhsum=0;
            for(int i=0;i<100;i++){
                dhsum += dh[i];
            }
            for(int i=100;i<dh.size();i++){
                corr_f.push_back(dhsum/100);
                dhsum=dhsum-dh[i-100]+dh[i];
            }
            cout<<corr_f.size()<<endl;
            double CF = corr_f[0];
            for(int i=0;i<corr_f.size();i++){
                    if(corr_f[i]>CF) CF=corr_f[i];
            }
            ui->CF_label->setText(QString::number(CF,'g',6));
//            base_timer->stop();
//            for(int i=0;i<20;i++){
//                cout<<feature[i]<<endl;
//            }
        }
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

void MainWindow::serial_getData()
{
    GPS_timer->start(GPS_timer_time); // 开始接收GPS信号之后延时500ms
}

void MainWindow::GPS_slot()
{ //读取串口缓存并处理
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
        if(save_status == true)
        {
            *GPSout <<"basetime:"<<base_time<<"\n";
            *GPSout<<"basetime:"<<base_time<<"\n";
            *GPSout<<"latitude:"<<gpsInfo.gprmc.NorS<<" "<<gpsInfo.gprmc.Latitude<<"\n";
            *GPSout<<"longitude:"<<gpsInfo.gprmc.EorW<<" "<<gpsInfo.gprmc.Longitude<<"\n";
            *GPSout<<"speed:"<<gpsInfo.gprmc.Speed<<"\n";
            *GPSout<<"GPS_accuracy:"<<gpsInfo.gpgga.HDOP<<"\n";
        }
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

void MainWindow::on_SAVE_button_clicked()
{
    //make a new directory
    myDir = new QDir();
    dirname = nowtime.toString("yyyyMMdd_hh:mm:ss");
    if(!myDir->mkdir(dirname))
        QMessageBox::information(this,tr("DIR"),"Can not make a new directory!");
    else{
        //files
        GPSfile = new QFile(dirname+"/GPS"+QString::number(file_num,10)+".txt");
        ACCfile = new QFile(dirname+"/ACC"+QString::number(file_num,10)+".txt");
        if(!GPSfile->open(QIODevice::WriteOnly | QIODevice::Text)||
                !ACCfile->open(QIODevice::WriteOnly | QIODevice::Text))
            QMessageBox::information(this,tr("FILE"),"Can not open gps or acc file!");
        else{

            ui->SAVE_button->setEnabled(false);
            ui->save_stop_button->setEnabled(true);
            GPSout = new QTextStream(GPSfile);
            ACCout = new QTextStream(ACCfile);
            save_status = true;
        }
    }
}
void MainWindow::on_save_stop_button_clicked()
{
    ui->SAVE_button->setEnabled(true);
    ui->save_stop_button->setEnabled(false);
    GPSfile->close();
    ACCfile->close();
    save_status = false;
}

void MainWindow::closeEvent(QCloseEvent *event){
    QMessageBox::information(this,tr("CLOSE"),"Are you sure to close the program?");
    GPSfile->close();
    ACCfile->close();
    ACC_serial->close();
    GPS_serial->close();
}


