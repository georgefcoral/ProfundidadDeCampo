#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <sortContours.h>
#include <cstring>
#include <iostream>

using namespace std;
using namespace cv;

int main (int argc, char **argv)
{
   char fileName1[256], fileName2[256], buff[256];
   Mat Image1, Image2, I1, I2;
   vector < vector < Point > >contours1, sContours1;
   vector < vector < Point > >contours2, sContours2;
   unsigned int nContours1, nContours2;
   unsigned int i, j, k;

   if (argc < 2)
   {
      cerr << "Faltan parámetros.\n\n\tUso:"
         << "\n\n\t\ttestMomentsMetricB ImageFileName1 ImageFileName2\n\n";
      exit (1);
   }
   strncpy (fileName1, argv[1], 255);
   strncpy (fileName2, argv[2], 255);

   Image1 = imread (fileName1, IMREAD_GRAYSCALE);
   Image2 = imread (fileName2, IMREAD_GRAYSCALE);

   if (Image1.empty() || Image2.empty())
   {
      cerr << "No se pudo abrir la imagen "  
           << ". Terminando el programa." << endl;
      exit(1);
   }
   threshold (Image1, Image1, 25, 255, THRESH_BINARY_INV);
   threshold (Image2, Image2, 25, 255, THRESH_BINARY_INV);
   findContours (Image1, contours1, RETR_EXTERNAL, CHAIN_APPROX_NONE);
   findContours (Image2, contours2, RETR_EXTERNAL, CHAIN_APPROX_NONE);

   filterSmallContours(contours1, 50);
   filterSmallContours(contours2, 50);

   nContours1 = contours1.size();
   nContours2 = contours2.size();

   if (!nContours1 || !nContours2)
   {
      cerr << "No se encontraron contornos. Terminando el programa"<< endl;
      exit(1);
   }

   sortContours (contours1, sContours1);
   nContours1 = sContours1.size();

   sortContours (contours2, sContours2);
   nContours2 = sContours2.size();

   namedWindow ("Contornos1", 1);
   namedWindow ("Contornos2", 1);

   for (i = 0; i < nContours1; ++i)
   {
      double dMin=0;
      unsigned int jMin;
      Moments mo1, mo2;
      double Hu1[7], Hu2[7];

      mo1 = moments (sContours1[i]);
      HuMoments (mo1, Hu1);
//      normHuMoments(Hu1);
      for (j = i; j < nContours2; ++j)
      {
         double d;

         mo2 = moments (sContours2[j]);
         HuMoments (mo2, Hu2);
       //  normHuMoments(Hu2);
         cout << "Hu1: [";
         for (k = 0; k < 6; k++)
            cout<< Hu1[k] <<", ";
         cout<< Hu1[6] <<"]" << endl;
         cout << "Hu2: [";
         for (k = 0; k < 6; k++)
            cout<< Hu2[k] <<", ";
         cout<< Hu2[6] <<"]" << endl << endl;

         d = 0.;
         for (k = 0; k < 7; k++)
            d += pow(Hu1[k] - Hu2[k], 2.);
         if (j!=i)
         {
            if (d < dMin)
            {
               jMin = j;
               dMin = d;
            }
         }
         else
         {
            dMin = d;
            jMin = i;
         }
         snprintf(buff, 255, "D(%02d, %02d) = %f", i, j, d);

         cvtColor (Image1, I1, COLOR_GRAY2RGB);
         cvtColor (Image2, I2, COLOR_GRAY2RGB);
         drawContours (I1, sContours1, i, Scalar (000, 000, 255), 2);
         drawContours (I2, sContours2, j, Scalar (255, 000, 000), 2);
         putText(I1, String(buff), Point2i (50, I1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
         putText(I2, String(buff), Point2i (50, I2.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
         imshow("Contornos1", I1);
         imshow("Contornos2", I2);
         if (waitKeyEx(0) == 27)
         {
            i = nContours1;
            break;
         }
      }
      cvtColor (Image1, I1, COLOR_GRAY2RGB);
      cvtColor (Image2, I2, COLOR_GRAY2RGB);
      drawContours (I1, sContours1, i, Scalar (000, 000, 255), 2);
      drawContours (I2, sContours2, jMin, Scalar (255, 000, 000), 2);
      snprintf(buff, 255, "DMin(%02d, %02d) = %f", i, jMin, dMin);
      putText(I1, String(buff), Point2i (50, I1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
      putText(I2, String(buff), Point2i (50, I2.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
      imshow("Contornos1", I1);
      imshow("Contornos2", I2);
      if (waitKeyEx(0) == 27)
         break;
   }
   destroyAllWindows();
   return 0;
}
