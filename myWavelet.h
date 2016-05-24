/*************************************************************************
	> File Name: myWavelet.h
	> Author: zyx 
	> Mail: zhang-yx10@foxmail.com
	> Created Time: Tuesday, May 17, 2016 PM03:18:27 HKT
 ************************************************************************/

#ifndef _MYWAVELET_H
#define _MYWAVELET_H
#include <vector>
#include <iostream>

// db3
#define Lo_R  {0.3327,0.8069,0.4599,-0.1350,-0.0854,0.0352};
#define Hi_R  {0.0352,0.0854,-0.1350,-0.4599,0.8069,-0.3327};
#define Lo_D  {0.0352,-0.0854,-0.1350,0.4599,0.8069,0.3327};
#define Hi_D  {-0.3327,0.8069,-0.4599,-0.1350,0.0854,0.0352};

bool mydwt(std::vector<double> &s,std::vector<double> &cA,std::vector<double> &cD);
bool mywaveDec(std::vector<double> &s,int N,std::vector<double> &C,std::vector<int> &L);
bool myupsconv(std::vector<double> &x,double *f,int l);
bool mywrcoef(char o,std::vector<double> &C,std::vector<int> &L,int N,std::vector<double> &X);
#endif
