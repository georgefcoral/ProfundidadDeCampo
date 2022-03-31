//g++ contornos.cpp -o contornos `pkg-config --cflags --libs opencv4`

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/types.hpp>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <random>
#include <iomanip>
#include <unistd.h>
//#include <sortContours.h>


using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);

const int umbralArea = 10;
int main (int argc, char **argv)
{
   ifstream infile;
   int t = 0;
   String dataFiles = String (argv[1]);
   infile.open (dataFiles);

    Mat frame;
    namedWindow("out",1);

   t = 0;
   while (getline (infile, file))
   {
      istringstream iss (file);
      frame = imread (file, IMREAD_GRAYSCALE);
      vector<vector<Point> > contours;
      findContours (frame,contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
      Mat frameRGB;
      cvtColor (frame,frameRGB, COLOR_GRAY2RGB);
      drawContours(frameRGB,contours,-1,Scalar(0,255,0));
      cout<<"contornos/"<<file<<endl;
      imshow("out",frameRGB);
      waitKey(0);
      //imwrite("contours/"+file,frameRGB);
      //filterSmallContours(tmpContours, umbralArea);
      //sortContours (tmpContours,contours);
      t++;
   }

   infile.close ();
   return 0;
}

