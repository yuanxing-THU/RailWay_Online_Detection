#include"accDisplay.h"

accDisplay::accDisplay(QWidget * parent):
        QLabel(parent)
{
    setObjectName("gllabel");
    setGeometry(QRect(480,150, 50, 50));
    ACC_status=false;
}
void accDisplay::paintEvent(QPaintEvent *){
    QPainter painter(this);
    if(ACC_status){
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
    }else{
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
    }
    //painter.drawText(rect(), Qt::AlignCenter, "Qt");
    painter.drawEllipse(0,0,20,20);
}
