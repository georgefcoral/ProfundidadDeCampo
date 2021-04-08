#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/types.hpp>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iomanip>
#include <unistd.h>
#include<fftw3.h>
#include <FourierDescriptor.h>



using namespace cv;
using namespace std;

String image_path;
String file;
const String outDir = "Results/";
RNG rng (12345);
Mat cambia (Mat M)              //Imagen Negativa.
{
   int i, j;
   double *ptr;

   for (i = 0; i < M.rows; ++i)
   {
      ptr = M.ptr < double >(i);
      for (j = 0; j < M.cols; ++j, ++ptr)
         *ptr = -*ptr + 255;
   }
   return M;
}



int main (void)
{
   int t = 0;
   int dilation_type = 2;
   int dilation_size = 5;

   String dataFiles = "myFile.txt";
   ifstream infile (dataFiles);
   Mat image;
   vector < vector < Point > >contours;
   vector < Vec4i > hierarchy;
/*FOURIER DESCRIPTORS VARIABLES*/
   
   unsigned int nDesc = 0;
   double reconError;



   cout << "Finding descriptors... " << endl;

   while (getline (infile, file))
   {
      istringstream iss (file);
      cout << file << endl;
      image_path = "Outputs/" + file;

      Mat image = imread (image_path, IMREAD_GRAYSCALE);
      if (!image.data)
      {
         cout << "Could not open or find the image" << std::endl;
         return -1;
      }


      Mat element = getStructuringElement (dilation_type,
                                           Size (2 * dilation_size + 1,
                                                 2 * dilation_size + 1),
                                           Point (dilation_size,
                                                  dilation_size));

      dilate (image, image, element);

      findContours (image, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Para HU Moments

      Mat imContours;
    /*Fourier Descriptors */
      
      {
         FourierDescriptor FD;

         Mat contoursFourier = image.clone ();
         cout << "Aqui:1" << endl;
         FD.setContours (contoursFourier);
         cout << "Aqui:2" << endl;
         FD.computeDescriptors ();
         cout << "Aqui:3" << endl;
         nDesc = contours.size ();
         cout << "Aqui:4" << endl;
         reconError = FD.reconstructContours (0.01);
         cout << "Aqui:5" << endl;
      
         plotContours (contoursFourier, imContours, FD);
      }
      namedWindow ("Output", 1);
      imshow ("Output", imContours);
      waitKey (0);
      if (waitKeyEx (30) > 0)
         break;
   
      Mat drawing = Mat::zeros (image.size (), CV_8UC3);
      //drawing contours.
      for (unsigned int i = 0; i < contours.size (); i++)
      {
         Scalar color =
            Scalar (rng.uniform (0, 256), rng.uniform (0, 256),
                    rng.uniform (0, 256));
         drawContours (drawing, contours, (int) i, color, 2, LINE_8,
                       hierarchy, 0);
      }

      imwrite (outDir + "contour" + file, drawing);
      //Variables para descriptores de momentos.
      vector < Moments > mu (contours.size ());
      vector < double[7] > huMoments (contours.size ());
      //Variables para descriptores de fourier.

      for (unsigned int i = 0; i < contours.size (); i++)
      {
         mu[i] = moments (contours[i]);
         HuMoments (mu[i], huMoments[i]);
         cout << "Momentos invariantes de Hu del objeto " << i << " : " <<
            endl << "[";

         //Escalamos momentos de Hu.
         //(de acuerdo a: https://learnopencv.com/shape-matching-using-hu-moments-c-python/)
         for (unsigned int j = 0; j < 7; ++j)
         {
            huMoments[i][j] =
               -1 * copysign (1.0,
                              huMoments[i][j]) *
               log10 (abs (huMoments[i][j]));
            cout << huMoments[i][j];
            if (j < 6)
               cout << ", ";
            else
               cout << "]" << endl;
         }

      }
      cout << endl;

      cout << "Secuencia: " << t << " con : " << contours.
         size () << " objetos." << endl;
      t++;

   }
   cout << "Terminado" << endl;
   infile.close ();

   return 0;
}
