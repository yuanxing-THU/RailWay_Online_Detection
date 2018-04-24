#include"FFT.h"

bool FFT(complex<double> *TD,complex<double> *FD,int r)
{
    int count;
    int i,j,k;
    int bfsize,p;
    double angle;
    complex<double> *W,*X1,*X2,*X;

    count = 1<<r;

    W = new complex<double>[count / 2];
    X1 = new complex<double>[count];
    X2 = new complex<double>[count];

    for(i=0;i<count/2;i++)
    {
        angle = -i*PI*2/count;
        W[i] = complex<double>(cos(angle),sin(angle));
    }
    memcpy(X1,TD,sizeof(complex<double>)*count);

    for(k=0;k<r;k++)
    {
        for(j=0;j<1<<k;j++)
        {
            bfsize = 1<<(r-k);
            for(i=0;i<bfsize/2;i++)
            {
                p=j*bfsize;
                X2[i+p] = X1[i+p]+X1[i+p+bfsize/2];
                X2[i+p+bfsize/2] = (X1[i+p]-X1[i+p+bfsize/2])*W[i*(1<<k)];
            }
        }
        X = X1;
        X1 = X2;
        X2 = X;
    }
    for(j=0;j<count;j++)
    {
        p=0;
        for(i=0;i<r;i++)
        {
            if(j&(1<<i))
            {
                p+=1<<(r-i-1);
            }
        }
        FD[j]=X1[p];
    }
    delete W;
    delete X1;
    delete X2;
    return true;
}

