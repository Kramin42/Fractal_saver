/*This source code copyrighted by Lazy Foo' Productions (2004-2012)
and may not be redestributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

//other constants
//const float PI = atan(1.0f) * 4.0f;

//Uint32 data[SCREEN_WIDTH*SCREEN_HEIGHT];


//    int iValue = (int) value;
//    if (((double) rand()/(double) RAND_MAX)>(value - (double) iValue))
//    {
//        iValue++;
//    }
//    return iValue;
//}

//default values
double M_centerX = -0.75;
double M_centerY = 0.0;
double M_zoom = 1.0;
int M_maxIteration = 200;
int M_ColMult = 1;
double M_bailoutRad = 16.0;
double BRsq=M_bailoutRad*M_bailoutRad;
double transformPower = 0.4;
bool distanceDivide = false;
double ddA = 0.4;
double ddB = 0.5;

double cx,cy;
double x,y,xsq,ysq;
double mu;
double divider;
double q;
int iteration;
Uint32 color;
double R,G,B;

//usefull vars
int i,j;
int w=800,h=600;
double wd=w,hd=h;

int progressCounter=0;
int progressCounterDepth = 5;


float getColorValue(double mu, double s, double m, double e)
{
    if (mu<s) {
        return 0.0f;
    } else if (mu<m) {
        return (mu - s)/(m-s);
    } else if (mu<e) {
        return 1.0f - (mu - m)/(e-m);
    }
    return 0.0f;
}

Uint32 getUintfromRGB(Uint8 r, Uint8 g, Uint8 b)
{
    return b + (g<<8) + (r<<16) + 0xFF000000;
}

Uint32 Mandelbrot(int i, int j){//calculates the values for the mandelbrot set
    //cout<<"test: entering Mandelbrot function"<<endl;
    cx = (((double) i)/wd)*(2.0/M_zoom)*(wd/hd) - ((1.0/M_zoom)*(wd/hd)-M_centerX);
    cy = (((double) j)/hd)*(2.0/M_zoom) - ((1.0/M_zoom)+M_centerY);

    x = 0;
    y = 0;
    xsq=0;
    ysq=0;

    iteration = 0;

//    if ((cx+1)*(cx+1)+cy*cy<1.0f/16.0f){
//        iteration = M_maxIteration;
//    }
//    q = (cx-0.25)*(cx-0.25)+cy*cy;
//    if (q*(q+(cx-0.25))<cy*cy/4.0f){
//        iteration = M_maxIteration;
//    }

    while (xsq + ysq <= BRsq && iteration < M_maxIteration)
    {
        y = 2*x*y + cy;
        x = xsq - ysq + cx;

        xsq=x*x;
        ysq=y*y;

        iteration++;
    }

    if (iteration == M_maxIteration) {
        color = 0xFF000000;
    }
    else {
        mu = (double)iteration + 1 - log(log(sqrt(x*x + y*y)))/log(2.0);
        mu = mu/(double)M_maxIteration;\
        if (mu>=1.0f)
        {
            cout<<mu<<endl;
        }
        mu=pow(mu,transformPower);
        divider  = mu;
        mu = mu*M_ColMult;
        mu = mu - floor(mu);

        //mu=(double)(w*j + i)/(double)(w*h);

        R=0;
        G=0;
        B=0;

        B = getColorValue(mu,0,0.5,0.6);
        G = getColorValue(mu,0.3,0.5,0.7);
        R = getColorValue(mu,0.4,0.5,1);

        if (distanceDivide)
        {
            if (divider<ddA) {
                R=0;
                G=0;
                B=0;
            } else if (divider<ddB) {
                R*=(divider-ddA)/(ddB-ddA);
                G*=(divider-ddA)/(ddB-ddA);
                B*=(divider-ddA)/(ddB-ddA);
            }
        }

        color = getUintfromRGB((Uint8)(R*255),(Uint8)(G*255),(Uint8)(B*255));
        //color = 0xFFFFFFFF;
    }
    //cout<<"test: leaving Mandelbrot function"<<endl;
    return color;
}

bool recursiveMandelbrot(int x1, int y1, int x2, int y2, Uint32 *data, bool *done, int depth)
{
    if (depth==progressCounterDepth){
        progressCounter++;
        cout<<"\r                \r"<<progressCounter<<"/"<<pow(4,depth);
    }
    //cout<<"test: entering recursiveMandelbrot"<<endl;
    bool allSame = true;
    Uint32 prevColor = 0;

    //go around edge of the box. If they all have the same color then fill in the box with that color
    if (x2-x1>1 && y2-y1>1){//check if the box encloses area
        //cout<<"test: going around edge of box"<<endl;
        //left edge
        i=x1;
        for (j=y1;j<=y2;j++){
            if (!done[j*w+i]){
                data[j*w+i]=Mandelbrot(i,j);
                done[j*w+i]=true;
            }
            //cout<<"test: color    : "<<data[j*w+i]<<endl;
            //cout<<"test: prevColor: "<<prevColor<<endl;
            //cout<<"test: colordiff: "<<!(data[j*w+i]==prevColor)<<endl;
            if (!(data[j*w+i]==prevColor) && j!=y1) {//check if the color is the same as the previous one. don't check for the first color
                allSame=false;
            }
            prevColor=data[w*j+i];
        }
        //cout<<"test3a"<<endl;
        //right edge
        i=x2;
        for (j=y1;j<=y2;j++){
            if (!done[j*w+i]){
                data[j*w+i]=Mandelbrot(i,j);
                done[j*w+i]=true;
            }
            if (!(data[j*w+i]==prevColor)) {//check if the color is the same as the previous one
                allSame=false;
            }
            prevColor=data[w*j+i];
        }
        //cout<<"test3b"<<endl;
        //top edge
        j=y1;
        for (i=x1+1;i<x2;i++){
            if (!done[j*w+i]){
                data[j*w+i]=Mandelbrot(i,j);
                done[j*w+i]=true;
            }
            if (!(data[j*w+i]==prevColor)) {//check if the color is the same as the previous one
                allSame=false;
            }
            prevColor=data[w*j+i];
        }
        //cout<<"test3c"<<endl;
        //bottom edge
        j=y2;
        for (i=x1+1;i<x2;i++){
            if (!done[j*w+i]){
                data[j*w+i]=Mandelbrot(i,j);
                done[j*w+i]=true;
            }
            if (!(data[j*w+i]==prevColor)) {//check if the color is the same as the previous one
                allSame=false;
            }
            prevColor=data[j*w+i];
        }
        //cout<<"test3d"<<endl;

        if (allSame) {//fill in the box
            if (depth<progressCounterDepth){
                progressCounter+=pow(4,progressCounterDepth-depth);
            }
            //cout<<"test: allSame found"<<endl;
            for (i=x1+1;i<x2;i++){
                for (j=y1+1;j<y2;j++){
                        data[j*w+i]=data[y1*w+x1];
                        //data[j*w+i]=0xFFFFFFFF;
                        done[j*w+i]=true;
                }
            }
        } else {//subdivide
            //cout<<"test: subdividing"<<endl;
            //cout<<"test: subdividing top left. x1: "<<x1+1<<", y1: "<<y1+1<<", x2: "<<x2/2<<", y2: "<<y2/2<<endl;
            recursiveMandelbrot(x1+1,y1+1,(x1+x2)/2,(y1+y2)/2,data,done,depth+1);//top left
            //cout<<"test: subdividing top right. x1: "<<x2/2+1<<", y1: "<<y1+1<<", x2: "<<x2-1<<", y2: "<<y2/2<<endl;
            recursiveMandelbrot((x1+x2)/2+1,y1+1,x2-1,(y1+y2)/2,data,done,depth+1);//top right
            //cout<<"test: subdividing bottom left. x1: "<<x1+1<<", y1: "<<y2/2+1<<", x2: "<<x2/2<<", y2: "<<y2-1<<endl;
            recursiveMandelbrot(x1+1,(y1+y2)/2+1,(x1+x2)/2,y2-1,data,done,depth+1);//bottom left
            //cout<<"test: subdividing bottom right. x1: "<<x2/2+1<<", y1: "<<y2/2+1<<", x2: "<<x2-1<<", y2: "<<y2-1<<endl;
            recursiveMandelbrot((x1+x2)/2+1,(y1+y2)/2+1,x2-1,y2-1,data,done,depth+1);//bottom right

        }
    } else {
        //cout<<"test: bottom of recursion"<<endl;
        for (i=x1;i<=x2;i++){
            for (j=y1;j<=y2;j++){
                if (!done[j*w+i]){
                    data[j*w+i]=Mandelbrot(i,j);
                    done[j*w+i]=true;
                }
            }
        }
    }


    return true;
}

int main( int argc, char *argv[] )
{
    cout << "argc = " << argc << endl;
    for(i = 0; i < argc; i++)
        cout << "argv[" << i << "] = " << argv[i] << endl;
    for (i=1;i<argc-1;i++)
    {
            if(!strcmp(argv[i], "-CX")){
                M_centerX = atof(argv[i+1]);
                cout << "CX: "<<M_centerX<<endl;
            }
            else if(!strcmp(argv[i], "-CY")){
                M_centerY = atof(argv[i+1]);
                cout << "CY: "<<M_centerY<<endl;
            }
            else if(!strcmp(argv[i], "-MI")){
                M_maxIteration = atoi(argv[i+1]);
                cout << "MI: "<<M_maxIteration<<endl;
            }
            else if(!strcmp(argv[i], "-CM")){
                M_ColMult = atoi(argv[i+1]);
                cout << "CM: "<<M_ColMult<<endl;
            }
            else if(!strcmp(argv[i], "-ZM")){
                M_zoom = atof(argv[i+1]);
                cout << "ZM: "<<M_zoom<<endl;
            }
            else if(!strcmp(argv[i], "-TP")){
                transformPower = atof(argv[i+1]);
                cout << "TP: "<<transformPower<<endl;
            }
            else if(!strcmp(argv[i], "-DD")){
                distanceDivide = (bool)atoi(argv[i+1]);
                cout << "DD: "<<distanceDivide<<endl;
            }
            else if(!strcmp(argv[i], "-DA")){
                ddA = atof(argv[i+1]);
                cout << "DA: "<<ddA<<endl;
            }
            else if(!strcmp(argv[i], "-DB")){
                ddB = atof(argv[i+1]);
                cout << "DB: "<<ddB<<endl;
            }
            else if(!strcmp(argv[i], "-IW")){
                w = atoi(argv[i+1]);
                wd = w;
                cout << "IW: "<<w<<endl;
            }
            else if(!strcmp(argv[i], "-IH")){
                h = atoi(argv[i+1]);
                hd = h;
                cout << "IH: "<<h<<endl;
            }

    }

    BRsq = M_bailoutRad*M_bailoutRad;

    SDL_Init( SDL_INIT_VIDEO );
    freopen("CON", "w", stdout);

    SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);

    Uint32 *data;
    data = new Uint32[w*h];

    bool *done;//array of booleans that tells if a cell has been calculated already
    done = new bool[w*h];

    //Algorithm for Mandelbrot set
//    for (i=0;i<w;i++){
//        for (j=0;j<h;j++){
//            x0 = (((double) i)/wd)*(2.0/M_zoom)*(wd/hd) - ((1.0/M_zoom)*(wd/hd)-M_centerX);
//            y0 = (((double) j)/hd)*(2.0/M_zoom) - ((1.0/M_zoom)+M_centerY);
//
//            x = 0;
//            y = 0;
//            xsq=0;
//            ysq=0;
//
//            iteration = 0;
//
//            if ((x0+1)*(x0+1)+y0*y0<1.0f/16.0f){
//                iteration = M_maxIteration;
//            }
//            q = (x0-0.25)*(x0-0.25)+y0*y0;
//            if (q*(q+(x0-0.25))<y0*y0/4.0f){
//                iteration = M_maxIteration;
//            }
//
//            while (xsq + ysq <= BRsq && iteration < M_maxIteration)
//            {
//                y = 2*x*y + y0;
//                x = xsq - ysq + x0;
//
//                xsq=x*x;
//                ysq=y*y;
//
//                iteration++;
//            }
//
//            if (iteration == M_maxIteration) {
//                color = 0xFF000000;
//            }
//            else {
//                mu = (double)iteration + 1 - log(log(sqrt(x*x + y*y)))/log(2.0);
//                mu = mu/(double)M_maxIteration;
//                if (mu>=1.0f)
//                {
//                    cout<<mu<<endl;
//                }
//                mu=pow(mu,transformPower);
//                divider  = mu;
//                mu = mu*M_ColMult;
//                mu = mu - floor(mu);
//
//                //mu=(double)(w*j + i)/(double)(w*h);
//
//                R=0;
//                G=0;
//                B=0;
//
//                B = getColorValue(mu,0,0.5,0.6);
//                G = getColorValue(mu,0.3,0.5,0.7);
//                R = getColorValue(mu,0.4,0.5,1);
//
//                if (distanceDivide)
//                {
//                    if (divider<ddA) {
//                        R=0;
//                        G=0;
//                        B=0;
//                    } else if (divider<ddB) {
//                        R*=(divider-ddA)/(ddB-ddA);
//                        G*=(divider-ddA)/(ddB-ddA);
//                        B*=(divider-ddA)/(ddB-ddA);
//                    }
//                }
//
//                color = getUintfromRGB((Uint8)(R*255),(Uint8)(G*255),(Uint8)(B*255));
//                //color = 0xFFFFFFFF;
//            }
//            data[j*w + i]=color;
//        }
//        cout<<"\r"<<i<<"/"<<w<<"        ";
//    }

    //cout<<"test1"<<endl;
    progressCounter=0;
    recursiveMandelbrot(0,0,w-1,h-1,data,done,0);

    Uint32 *pixels = (Uint32 *)image->pixels;

    for (i=0;i<w*h;i++)
    {
        pixels[i]=data[i];
    }

    SDL_SaveBMP(image,"fractal.bmp");

    delete [] data;
    delete [] done;
    data = NULL;
    done = NULL;

	return 0;
}
