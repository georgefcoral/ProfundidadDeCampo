#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <fstream> 
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/types.hpp>
#include <Mosaic.h>
#include <sys/timeb.h>
#include <algorithm> 

using namespace cv;
using namespace std;

const int alpha_slider_max = 27;
int alpha_slider;
double alpha;
double beta;
String names[alpha_slider_max];
int IM_WIDTH;
int IM_HEIGHT;
String dir;
/*
    Esta estructura define una clase que utiliza un vector de imágenes, en donde cada imagen es un ROI
    de una imagen mas grande para almacenar una pirámide de imágenes.

    El objetivo de hacer esto es contar con una forma fácil de visualizar la pirámide. 
*/
struct Pyramid
{
    vector <Mat> pyr;
    Mat repPyr;
    unsigned int numLevels;

    void setPyramid(const Mat &img, unsigned minSide = 4)
    {
        vector <Mat> tmp;
        Mat level;
        char org;

        // Asumiento que la imagen mas pequeña de la piramide tiene el lado mas pequeño igual
        // a minSide, el número de niveles de la pirámide es igual a
        // log_2(min(I.rows, I.cols)/minSide) + 1.
        if (img.cols > img.rows)
        {
            repPyr = Mat::zeros(Size (img.cols, (int)(img.rows * 1.5)),img.type());
            numLevels = (int)floor(log ( (double)img.rows / minSide) / log(2.))+ 1;
            org = 'w';
        }
        else
        {
            repPyr = Mat::zeros(Size ((int)(img.cols * 1.5), img.rows),img.type());
            numLevels = (int)floor(log ( img.rows/ 4.0) / log(2))+ 1;
            org = 'h';
        }

        if (numLevels != pyr.size())
            pyr.clear();
        

        buildPyramid(img, tmp, numLevels);

        level = repPyr(Rect(0,0,tmp[0].cols, tmp[0].rows));
        tmp[0].copyTo(level);
        pyr.push_back(level);
        if (tmp.size() > 1)
        {
            unsigned int i, pos;

            if (org == 'h')
            {
                level = repPyr(Rect(tmp[0].cols,0,tmp[1].cols, tmp[1].rows));
                tmp[1].copyTo(level);
                pyr.push_back(level);

                pos = tmp[1].rows;
                for (i = 2;i < tmp.size(); ++i)
                {
                    
                    level = repPyr(Rect(tmp[0].cols, pos,tmp[i].cols, tmp[i].rows));
                    tmp[i].copyTo(level);
                    pyr.push_back(level);
                    pos += tmp[i].rows;
                }
            }
            else
            {
                level = repPyr(Rect(0, tmp[0].rows,tmp[1].cols, tmp[1].rows));
                tmp[1].copyTo(level);
                pyr.push_back(level);

                pos = tmp[1].cols;
                for (i = 2; i < tmp.size(); ++i)
                {
                    level = repPyr(Rect(pos, tmp[0].rows, tmp[i].cols, tmp[i].rows));
                    tmp[i].copyTo(level);
                    pyr.push_back(level);
                    pos += tmp[i].cols;
                }
            }
        }
    }

    Pyramid ()
    {
        numLevels = 0;
    }

    Pyramid (const Mat &img, unsigned minSide = 4)
    {
        setPyramid (img, minSide);
    }
};


//Esta función invierte una imagen. La utilizamos únicamente para demostrar como aplicar métodos a la 
//a un nivel de la piramide. 
void cambia(Mat &M)
{
    int i, j;
    double *ptr;

    for (i=0;i<M.rows;++i)
    {
        ptr = M.ptr<double>(i);
        for (j=0;j<M.cols;++j, ++ptr)
            *ptr = -*ptr+255;
    }
}



static void on_trackbar( int, void* )
{
   Mat grayFrame, qframe, Out;
   Pyramid pyr;
   String image_path = dir + names[alpha_slider];
   Mat frame = imread(image_path, IMREAD_GRAYSCALE);
   
   IM_WIDTH = frame.cols;
   IM_HEIGHT = frame.rows;

   frame.convertTo (grayFrame, CV_64FC1);
   pyr.setPyramid(grayFrame);
   cambia(pyr.pyr[0]);
   //cout<<endl<<IM_WIDTH<<" X "<<IM_HEIGHT<<endl;
   pyr.repPyr.convertTo (Out, CV_8UC1);
   //resize(Out, qframe, Size(1500, 1000));
   imshow("Frame", Out);
}


int main(int argc, char **argv)
{

if (argc < 2)
   {
    cerr << "Faltan Parámetros." << endl;
    cerr << "Imagen de entrada." << endl << endl;
    exit(1);
    }

int t = 0;
String file;
dir = argv[1];//Dirección de imagenes 
String dataFiles ="myFile.txt";
cout<<dataFiles;
ifstream infile(dataFiles);
while (getline(infile,file)){
   istringstream iss(file);
   names[t]= file;
   //cout<<names[t];
   t++;
}
infile.close();
cout<<t<<endl;

alpha_slider = 0;
namedWindow("Frame", WINDOW_NORMAL); // Create Window
char TrackbarName[50];
sprintf( TrackbarName, "Frame max %d", alpha_slider_max );


createTrackbar( TrackbarName, "Frame", &alpha_slider, alpha_slider_max-1, on_trackbar );
on_trackbar( alpha_slider, 0 );
waitKey(0);
   
destroyAllWindows();
return 0;
}