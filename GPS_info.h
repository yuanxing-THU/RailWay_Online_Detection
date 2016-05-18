#ifndef GPS_INFO_H
#define GPS_INFO_H
//#include <QFile>
#include <QString>
#define N_UTCtime 9;
#define N_State 1;
#define N_Latitude 10;
#define N_NorS 1;
#define N_Longitude 10;
#define N_EorW 1;
#define N_Speed 7;
#define N_Direction 7;
#define N_UTCdate 6;
#define N_UTCtime 9;

#define N_MagVar 7;
#define N_Declination 1;
#define N_Mode 1;




/*
$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>,<14>*<15><CR><LF>
$GPGGA:头域
<1>~<14>:参数域
<CR><LF>:回车，换行。是相邻数据帧的分隔符
最大帧长72

<1> UTC时间，格式为hhmmss.ss。
<2> 纬度，格式为ddmm.mmmmm（前导位数不足则补0）。
<3> 纬度半球，N或S（北纬或南纬）。
<4> 经度，格式为dddmm.mmmmm（前导位数不足则补0）。
<5> 经度半球，E或W（东经或西经）。
<6> 定位质量指示，0=定位无效，1=定位有效。
<7> 使用卫星数量，从00到12（前导位数不足则补0）。
<8> 水平精确度，0.5到99.9。
<9> 天线离海平面的高度，-9999.9到99999.9米
<10> 高度单位，M表示单位米。
<11> 大地椭球面相对海平面的高度（-999.9到9999.9）。
<12> 高度单位，M表示单位米。
<13> 差分GPS数据期限（RTCM SC-104），最后设立RTCM传送的秒数量。
<14> 差分参考基站标号，从0000到1023（前导位数不足则补0）。
<15> 校验和。
*/

class GPGGA
{
public:
    //以下各数组存储了数据帧中的参数。数组大小N=参数长度L+1。
    //其中，Array[0]~Array[L-1]存储参数。Array[L]=Array[N-1]存储分隔符'\0'，代表数据帧中各参数间的分隔符(,)

    //QChar UTCtime[10];   //UTC时间:hhmmss.ss
    //QChar Latitude[11];  //纬度:ddmm.mmmmm
    //QChar NorS[2];       //纬度半球:N=北半球,S=南半球
    //QChar Longitude[12]; //经度:dddmm.mmmmm
    //QChar EorW[2];       //经度半球:E=东经,W=西经
    //QChar State[2];      //定位状态:0=定位无效，1=定位有效
    QChar Quantity[3];   //卫星数量:00~12
    QChar HDOP[5];       //水平精确度:0.5~99.9
    //QChar Altitude[8];   //海拔:-9999.9~99999.9
    //QChar AltiUnit[2];   //海拔单位：M=米
    //QChar WGS[7];        //WGS(世界测地系统)水准面划分:-999.9~9999.9
    //QChar WGSUnit[2];    //WGS水准面划分单位:M=米
    //QChar RTCM[];        //差分时间
    //QChar RTCMID[5];     //差分时间基站标号：0000~1023
};


/*
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13><CR><LF>
$GPRMC:头域
<1>~<12>:参数域
<CR><LF>:回车，换行。是相邻数据帧的分隔符
最大帧长70

<1> UTC（Coordinated Universal Time）时间，hhmmss.ss（时分秒）格式
<2> 定位状态，A=有效定位，V=无效定位
<3> Latitude，纬度ddmm.mmmmm（度分）格式（前导位数不足则补0）
<4> 纬度半球N（北半球）或S（南半球）
<5> Longitude，经度dddmm.mmmmm（度分）格式（前导位数不足则补0）
<6> 经度半球E（东经）或W（西经）
<7> 地面速率（000.0~999.9节，Knot，前导位数不足则补0）
<8> 地面航向（000.0~359.9度，以真北为参考基准，前导位数不足则补0）
<9> UTC日期，ddmmyy（日月年）格式
<10> Magnetic Variation，磁偏角（000.0~180.0度，前导位数不足则补0）
<11> Declination，磁偏角方向，E（东）或W（西）
<12> Mode Indicator，模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
*/

class GPRMC
{
public:
    //以下各数组存储了数据帧中的参数。数组大小N=参数长度L+1。
    //其中，Array[0]~Array[L-1]存储参数。Array[L]=Array[N-1]存储分隔符'\0'，代表数据帧中各参数间的分隔符(,)

    QChar UTCtime[10];    //UTC时间:hhmmss.ss（时分秒）
    QChar State[2];      //定位状态:A=有效定位，V=无效定位
    QChar Latitude[11];  //纬度:ddmm.mmmmm（度分）
    QChar NorS[2];       //纬度半球:N=北半球,S=南半球
    QChar Longitude[11]; //经度:dddmm.mmmmm（度分）
    QChar EorW[2];       //经度半球:E=东经,W=西经
    QChar Speed[8];      //地面速率:000.0~999.9节
    QChar Direction[8];  //地面航向:000.0~359.9度
    QChar UTCdate[7];    //UTC日期:ddmmyy（日月年）
    QChar MagVar[8];     //磁偏角:000.0~180.0度
    QChar Declination[2];//磁偏角方向:E=东,W=西
    QChar Mode[2];       //模式指示:A=自主定位，D=差分，E=估算，N=数据无效
};


class GPS_info
{
public:
    GPGGA gpgga;
    GPRMC gprmc;
    bool effective;
    void infoProcess(QString &rxdata);
};
#endif // GPS_INFO_H
