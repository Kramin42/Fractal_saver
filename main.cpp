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

int main( int argc, char *argv[] )
{
    //usefull vars
    int i,j;
    int w=800,h=600;
    double wd=w,hd=h;

    //default values
    double M_centerX = -0.75;
    double M_centerY = 0.0;
    double M_zoom = 1.0;
    int M_maxIteration = 200;
    int M_ColMult = 1;
    double M_bailoutRad = 3;
    double transformPower = 0.4;
    bool distanceDivide = false;
    double ddA = 0.4;
    double ddB = 0.5;

    cout << "argc = " << argc << endl;
    for(int i = 0; i < argc; i++)
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

    SDL_Init( SDL_INIT_VIDEO );
    freopen("CON", "w", stdout);

    SDL_Surface *image = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);

    Uint32 *data;
    data = new Uint32[w*h];

	double x0,y0;
    double x,y,xsq,ysq;
    //double xtemp;
    double mu;
    double divider;
    double q;
    double BRsq=M_bailoutRad*M_bailoutRad;
    int iteration;

    Uint32 color;
    //float H,S,B;
    double R,G,B;

    //Algorithm for Mandelbrot set
    for (i=0;i<w;i++){
        for (j=0;j<h;j++){
            x0 = (((double) i)/wd)*(2.0/M_zoom)*(wd/hd) - ((1.0/M_zoom)*(wd/hd)-M_centerX);
            y0 = (((double) j)/hd)*(2.0/M_zoom) - ((1.0/M_zoom)+M_centerY);

            x = 0;
            y = 0;
            xsq=0;
            ysq=0;

            iteration = 0;

            if ((x0+1)*(x0+1)+y0*y0<1.0f/16.0f){
                iteration = M_maxIteration;
            }
            q = (x0-0.25)*(x0-0.25)+y0*y0;
            if (q*(q+(x0-0.25))<y0*y0/4.0f){
                iteration = M_maxIteration;
            }

            while (xsq + ysq <= BRsq && iteration < M_maxIteration)
            {
                y = 2*x*y + y0;
                x = xsq - ysq + x0;

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
            data[j*w + i]=color;
        }
        cout<<"\r"<<i<<"/"<<w<<"        ";
    }

    Uint32 *pixels = (Uint32 *)image->pixels;

    for (i=0;i<w*h;i++)
    {
        pixels[i]=data[i];
    }



    SDL_SaveBMP(image,"fractal.bmp");

    delete [] data;
    data = NULL;

	return 0;
}
