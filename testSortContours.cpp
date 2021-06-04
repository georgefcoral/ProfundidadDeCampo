#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include <sortContours.h>
#include <cstring>
#include <iostream>

using namespace std;
using namespace cv;

int main (int argc, char **argv)
{
   char fileName[256];
   Mat Image;
   vector < vector < Point > >contours, sContours;
   unsigned int nContours, cont;


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

   sortContours (contours, sContours);

   namedWindow ("Contornos Originales", 1);
   namedWindow ("Contornos Ordenados", 1);

   cont = 0;
   nContours = sContours.size ();
   cout << "contours.size() = " << contours.size () << endl;
   cout << "sContours.size() = " << sContours.size () << endl;
   if (!nContours)
   {
      cerr << "No se encontraron contornos. Terminando el programa"<< endl;
      exit(1);
   }
   while (true)
   {
      Mat I1, I2;
      Point2f cM, cMs;
      Moments mo, mos;

      mo = moments (contours[cont]);
      mos = moments (sContours[cont]);

      cM = Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                    static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));
      cMs = Point2f (static_cast < float >(mos.m10 / (mos.m00 + 1e-5)),
                     static_cast < float >(mos.m01 / (mos.m00 + 1e-5)));


      cvtColor (Image, I1, COLOR_GRAY2RGB);
      cvtColor (Image, I2, COLOR_GRAY2RGB);

      drawContours (I1, contours, cont, Scalar (000, 000, 255), 2);
      drawContours (I2, sContours, cont, Scalar (255, 000, 000), 2);
      circle (I1, Point (cM.x, cM.y), 10, Scalar (0, 0, 255), true);
      circle (I2, Point (cMs.x, cMs.y), 10, Scalar (255, 0, 0), true);


      imshow ("Contornos Originales", I1);
      imshow ("Contornos Ordenados", I2);
      if (waitKeyEx(0) == 27)
         break;
      cont = (cont + 1) % nContours;
   }
   return 0;
}
