#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    FileStorage fs;
    Mat K, distCoeffs, Kn;
    Mat Mx, My;
    int width, height;

    if (argc < 2)
        return 1;

    fs.open(argv[1], FileStorage::READ);
    
    fs["Camera_Matrix"] >> K;
    fs["Distortion_Coefficients"] >> distCoeffs;
    fs["image_Width"] >> width;
    fs["image_Height"] >> height;
    Size ImageSize(width, height);

    cout << "K = " << K << endl << endl;
    cout << "distCoeffs = " << distCoeffs << endl << endl;

   Kn = getOptimalNewCameraMatrix(K, distCoeffs, ImageSize, 1, ImageSize, 0);
   initUndistortRectifyMap(K, distCoeffs, Mat::eye(3, 3, CV_32FC1), Kn, ImageSize, CV_16SC2, Mx, My);
    cout << "Kn = " << Kn << endl << endl;

   for (int i=2;i<argc;++i)
   {
      Mat I, O;
      char nameOut[256];

      I = imread(argv[i], 1);
      snprintf(nameOut, 255, "Out_%05d.png", i-2);
      remap (I, O, Mx, My, INTER_LINEAR, BORDER_TRANSPARENT);
      imwrite (nameOut, O);  
   }
    
    return 0;
}
            
