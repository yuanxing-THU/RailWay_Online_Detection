#ifndef ACCDISPLAY_H
#define ACCDISPLAY_H
#include<QLabel>
#include<QPainter>

class accDisplay:public QLabel
{
    //Q_OBJECT
public:
    explicit accDisplay(QWidget * parent=0);
    bool ACC_status;
protected:
    void paintEvent(QPaintEvent *);
};
#endif // ACCDISPLAY_H
