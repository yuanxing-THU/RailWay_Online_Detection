#ifndef GPSDISPLAY_H
#define GPSDISPLAY_H
#include<QLabel>
#include<QPainter>

class gpsDisplay:public QLabel
{
    //Q_OBJECT
public:
    explicit gpsDisplay(QWidget * parent=0);
    bool GPS_status;
protected:
    void paintEvent(QPaintEvent *);
};


#endif // GPSDISPLAY_H
