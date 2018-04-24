#include"storeDisplay.h"

storeDisplay::storeDisplay(QWidget * parent):
        QLabel(parent)
{
    setObjectName("gllabel");
    setGeometry(QRect(480,250, 50, 50));
    STORE_status=false;
}
void storeDisplay::paintEvent(QPaintEvent *){
    QPainter painter(this);
    if(STORE_status){
        painter.setPen(Qt::green);
        painter.setBrush(Qt::green);
    }else{
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
    }
    //painter.drawText(rect(), Qt::AlignCenter, "Qt");
    painter.drawEllipse(0,0,20,20);
}
