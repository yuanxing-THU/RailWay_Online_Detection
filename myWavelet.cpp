/*************************************************************************
	> File Name: myWavelet.cpp
	> Author: zyx 
	> Mail: zhang-yx10@foxmail.com
	> Created Time: Tuesday, May 17, 2016 PM03:21:38 HKT
 ************************************************************************/
#include<myWavelet.h>
#include<iostream>
#include<algorithm>
using namespace std;
//mydwt: single-level discrete 1-D wavelet transform
// s : the input signal vector ;
// cA: the output approximation coefficients vector 
// cD: the output detail coefficients vector 
bool mydwt(vector<double> &s,vector<double> &cA,vector<double> &cD){
    int l = s.size();
    vector<double> * y;
    y=new vector<double>(s);
    cA.clear();
    cD.clear();
    int i=1;
    for(i=0;i<5;i++){
        y->insert(y->begin(),s[i]);
        y->push_back(s[l-1-i]);
    }

    int up=(l+5)/2;
    double temp;
    double Lo[6]=Lo_R;
    double Hi[6]=Hi_R;
    int k=0;
    for(i=0;i<up;i++){
        temp=0;
        for(k=0;k<6;k++){
            temp+=Lo[k]*(y->at(2*i+1+k));
        }
        cA.push_back(temp);
    }
    for(i=0;i<up;i++){
        temp=0;
        for(k=0;k<6;k++){
            temp+=Hi[k]*(y->at(2*i+1+k));
        }
        cD.push_back(temp);
    }
    delete y;
    return true;
}

//mywaveDec : multilevel 1-D wavelet decomposition
//s : the input signal vector
//N : the levels 
//C : the output coefficients [An Dn Dn-1...D2 D1]
//L : the lengths [lAn lDn lDn-1 ... ls]
bool mywaveDec(vector<double> &s,int N,vector<double> &C,vector<int> &L){
    if(N<1) return false;
    C.clear();
    L.clear();
    
    int l=s.size();
    L.push_back(l);
    vector<double> cA,cD;
    vector<double> cAx,cDx;

    mydwt(s,cA,cD);
    L.insert(L.begin(),cD.size());
    C.insert(C.begin(),cD.begin(),cD.end());
    int k;
    if(N>1){
        for(k=2;k<=N;k++){
            mydwt(cA,cAx,cDx);
            C.insert(C.begin(),cDx.begin(),cDx.end());
            L.insert(L.begin(),cDx.size());
            cA.clear();
            cA.insert(cA.begin(),cAx.begin(),cAx.end());
            cAx.clear();
            cDx.clear();
        }
    }
    C.insert(C.begin(),cA.begin(),cA.end());
    L.insert(L.begin(),cA.size());

    return true;
}

//myupsconv : Upsample and convolution 1D 
//x : the input signal coefficients 
//f : the db3 signal coefficients
//l : the length upsampling target
//y : the output signal 

bool myupsconv(vector<double> &x,double *f,int l,vector<double> &y){
    y.clear();
    vector<double> z;
    int i=0;
    for(i=0;i<2*x.size()-1;i++){
        z.push_back(0);
    }
    for(i=0;i<x.size();i++){
        z[2*i]=x[i];
    }
    for(i=0;i<5;i++){
        z.insert(z.begin(),0);
        z.push_back(0);
    }

    int k=0;
    double tempy;
    for(i=0;i<z.size()-5;i++){
        tempy=0;
        for(k=0;k<6;k++){
            tempy+=f[k]*z[i+k];
        }
        y.push_back(tempy);
    }
    for(i=0;i<4;i++){
        y.erase(y.begin());
    }
    while(y.size()>l){
        y.erase(y.begin()+l);
    }
    return true;
}

//mywrcoef: Reconstruct single branch from 1-D wavelet coefficients
//o : 'a' or 'd'  approximation or detail 
//C : the whole coefficents which is the output of mywaveDec() 
//L : the lengths which is the output of mywaveDec()
//N : the level that will be reconstruct
//X : the output signal
bool mywrcoef(char o,vector<double> &C,vector<int> &L,int N,vector<double> &X){
    X.clear();
    double Lo[6]=Lo_D;
    double Hi[6]=Hi_D;
    int n;
    double *f;
    
    if(o=='a'){
        n=L.size()-2;
        f=Lo;
        X.insert(X.begin(),C.begin(),C.begin()+L[0]-1);
    }else{
        n=N;
        f=Hi;
        int j=L.size()-N-1;
        int k=0,b=0;
        while(k<j){
            b+=L[k];
            k++;
        }
        X.insert(X.begin(),C.begin()+b,C.begin()+b+L[j]);
    }
    int nmin = L.size()-n;
    vector<double> tempX;
    myupsconv(X,f,L[nmin],tempX);
    X.clear();
    X.insert(X.begin(),tempX.begin(),tempX.end());
    int k;
    for(k=nmin+2;k<=L.size();k++){
        tempX.clear();
        myupsconv(X,Lo,L[k-1],tempX);
        X.clear();
        X.insert(X.begin(),tempX.begin(),tempX.end());
    }
}

