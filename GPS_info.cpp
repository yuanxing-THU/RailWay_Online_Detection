#include"GPS_info.h"
#include<algorithm>
#include <QTextStream>
#include <QString>
#include <iostream>
void GPS_info::infoProcess(QString &rxdata){
    if(rxdata.size()<100) {
        effective = false;
        return;
    }
    QString mc="$GPRMC";
    QChar* index;
    int i,k;
    index=std::search(rxdata.begin(),rxdata.end(),mc.begin(),mc.end());
    if(*index == *(rxdata.end())){
        effective = false;
        return;
    }
    //QTextStream out(stdout);
    //out<<"*index :"<<QChar(*index)<<*(index+1)<<*(index+4)<<endl;
    i=0;k=0;
    while(*(index+i)!=',') i++;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.UTCtime[k] = *(index+i+k+1); k++;}
    gprmc.UTCtime[k]='\0';

    //out<<"gprmc.UTCtime :"<<*gprmc.UTCtime<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.State[k] = *(index+i+k+1); k++;}
    gprmc.State[k]='\0';
    //out<<"gprmc.State :"<<*gprmc.State<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.Latitude[k] = *(index+i+k+1); k++;}
    gprmc.Latitude[k]='\0';
    //out<<"gprmc.Latitude :"<<*gprmc.Latitude<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.NorS[k] = *(index+i+k+1); k++;}
    gprmc.NorS[k]='\0';
    //out<<"gprmc.NorS :"<<*gprmc.NorS<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.Longitude[k] = *(index+i+k+1); k++;}
    gprmc.Longitude[k]='\0';
    //out<<"gprmc.Longitude :"<<*gprmc.Longitude<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.EorW[k] = *(index+i+k+1); k++;}
    gprmc.EorW[k]='\0';
    //out<<"gprmc.EorW :"<<*gprmc.EorW<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.Speed[k] = *(index+i+k+1); k++;}
    gprmc.Speed[k]='\0';
    //out<<"gprmc.Speed :"<<*gprmc.Speed<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.Direction[k] = *(index+i+k+1); k++;}
    gprmc.Direction[k]='\0';
    //out<<"gprmc.Direction :"<<*gprmc.Direction<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.UTCdate[k] = *(index+i+k+1); k++;}
    gprmc.UTCdate[k]='\0';
    //out<<"gprmc.UTCdate :"<<*gprmc.UTCdate<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.MagVar[k] = *(index+i+k+1); k++;}
    gprmc.MagVar[k]='\0';
    //out<<"gprmc.MagVar :"<<*gprmc.MagVar<<endl;
    i+=k+1;
    k=0;
    while(*(index+i+k+1)!=',') {if(index+i+k+1==rxdata.end()) {effective = false; return;} gprmc.Declination[k] = *(index+i+k+1); k++;}
    gprmc.Declination[k]='\0';
    //out<<"gprmc.Declination :"<<*gprmc.Declination<<endl;
    i+=k+1;
    k=0;
    gprmc.Mode[0]=*(index+i+k+1);
    gprmc.Mode[1]='\0';
    //out<<"gprmc.Mode :"<<*gprmc.Mode<<endl;


    QString ga="$GPGGA";
    index=std::search(rxdata.begin(),rxdata.end(),ga.begin(),ga.end());
    if(*index == *(rxdata.end())){
        effective = false;
        return;
    }
    i=0;k=7;
    while(k){
        if(*(index+i)==',') k--;
        i++;
    }
    k=0;
    while(*(index+i+k)!=',') {if(index+i+k==rxdata.end()) {effective = false; return;} gpgga.Quantity[k] = *(index+i+k); k++;}
    gpgga.Quantity[k]='\0';
    i+=k+1;
    k=0;
    while(*(index+i+k)!=',') {if(index+i+k==rxdata.end()) {effective = false; return;} gpgga.HDOP[k] = *(index+i+k); k++;}
    gpgga.HDOP[k]='\0';
    effective = true ;
    return;
}
