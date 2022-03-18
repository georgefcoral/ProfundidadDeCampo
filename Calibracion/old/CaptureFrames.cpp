/**
\file fitPlane.cpp
*/
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <Mosaic.h>
#include <vector>
#include <list>
#include <vector>
#include <string>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    Mat gFrame, cFrame, Corners;
    int i, width = 640, height = 480, delay = 33;
    char basename[64] = "Frame", name[256];

    if (argc < 2)
        return 1;

    if (argc > 2)
        strncpy (basename, argv[2],63);
    if (argc > 3)
        width = atoi(argv[3]); 
    if (argc > 4)
        height = atoi(argv[4]); 
    if (argc > 5)
        delay = atoi(argv[5]); 

    VideoCapture cap(atoi(argv[1])); // open the camera

    cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        
    cout << "Image size: [" 
         << cap.get(CV_CAP_PROP_FRAME_WIDTH ) << ", " 
         << cap.get(CV_CAP_PROP_FRAME_HEIGHT) << "]" << endl;
    cout.flush();

    if(!cap.isOpened())  // check if we succeeded
        return -1;
    namedWindow( "Frame", 1 );

    for (i = 0;;++i)
    {
       //Capturamos una imagen, y validamos que haya funcionado la operacion.
        cap.grab();
        cap.retrieve(cFrame);
        if (cFrame.empty() )
            break;

        imshow ("Frame", cFrame);
        snprintf(name, 255,"%s_%04d.png", basename, i);
        imwrite (name, cFrame);
        if (waitKeyEx(delay) >= 0)
            break;
    }
    destroyAllWindows();

    return 0;
}
