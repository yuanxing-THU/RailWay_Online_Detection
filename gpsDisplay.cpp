#include"gpsDisplay.h"

gpsDisplay::gpsDisplay(QWidget * parent):
        QLabel(parent)
{
    setObjectName("gllabel");
    setGeometry(QRect(480,60, 50, 50));
    GPS_status=false;
}
void gpsDisplay::paintEvent(QPaintEvent *){
    QPainter painter(this);
    if(GPS_status){
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
    }else{
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
    }
    //painter.drawText(rect(), Qt::AlignCenter, "Qt");
    painter.drawEllipse(0,0,20,20);
}
