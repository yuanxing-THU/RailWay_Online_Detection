#ifndef STOREDISPLAY_H
#define STOREDISPLAY_H
#include<QLabel>
#include<QPainter>
class storeDisplay:public QLabel
{
    //Q_OBJECT
public:
    explicit storeDisplay(QWidget * parent=0);
    bool STORE_status;
protected:
    void paintEvent(QPaintEvent *);
};
#endif // STOREDISPLAY_H
