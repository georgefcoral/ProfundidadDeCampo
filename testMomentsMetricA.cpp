#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <sortContours.h>
#include <cstring>
#include <iostream>

using namespace std;
using namespace cv;

int main (int argc, char **argv)
{
   char fileName[256], buff[256];
   Mat Image, I1;
   vector < vector < Point > >contours, sContours;
   unsigned int nContours;
   unsigned int i, j, k;

   if (argc < 2)
   {
      cerr << "Faltan parámetros.\n\n\tUso:"
         << "\n\n\t\ttestSortContours ImageFileName\n\n";
      exit (1);
   }
   strncpy (fileName, argv[1], 255);

   Image = imread (fileName, IMREAD_GRAYSCALE);
   if (Image.empty())
   {
      cerr << "No se pudo abrir la imagen " << fileName
           << ". Terminando el programa." << endl;
      exit(1);
   }
   threshold (Image, Image, 25, 255, THRESH_BINARY_INV);
   findContours (Image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

   filterSmallContours(contours, 50);

   nContours = contours.size();

   if (!nContours)
   {
      cerr << "No se encontraron contornos. Terminando el programa"<< endl;
      exit(1);
   }

   sortContours (contours, sContours);
   nContours = sContours.size();

   namedWindow ("Contornos", 1);

   for (i = 0; i < nContours; ++i)
   {
      Moments mo1, mo2;
      double Hu1[7], Hu2[7];

      mo1 = moments (sContours[i]);
      HuMoments (mo1, Hu1);
      normHuMoments(Hu1);
      for (j = i; j < nContours; ++j)
      {
         double d;

         mo2 = moments (sContours[j]);
         HuMoments (mo2, Hu2);
         normHuMoments(Hu2);
         d = 0.;
         for (k = 0; k < 7; k++)
            d += pow(Hu1[k] - Hu2[k], 2.);
         snprintf(buff, 255, "D(%02d, %02d) = %f", i, j, d);

         cvtColor (Image, I1, COLOR_GRAY2RGB);
         drawContours (I1, sContours, i, Scalar (000, 000, 255), 2);
         drawContours (I1, sContours, j, Scalar (255, 000, 000), 2);
         putText(I1, String(buff), Point2i (50, I1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
         imshow("Contornos", I1);
         if (waitKeyEx(0) == 27)
         {
            i = nContours;
            break;
         }
      }
   }
   destroyAllWindows();
   return 0;
}
