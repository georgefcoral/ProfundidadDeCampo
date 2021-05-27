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
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iomanip>
#include <unistd.h>
#include<fftw3.h>
#include <FourierDescriptor.h>
#include <frameData.h>
#include "deepFunctions2.h"

using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);

int main (int argc, char **argv)
{
   //vector <vector < Point2i >> mcGlobal(100);
   vector <Mat> mcGlobal(100);
   Mat pointMat;
   int rowsGlobal,colsGlobal;
   const int umbralArea = 450;
   float maxCorrespondences = 0;
   int t = 0;
   int dilation_type = 2;
   int dilation_size = 2;
   bool Umbraliza = true;
   double umbralDistance = 60.;
   double umbralHu= 500.;
   vector<frameData> Frames; 

  
   String dataFiles, OutDir;
   ifstream infile;
   Mat image;
   vector < vector < Point > >contours;

   unsigned int nDesc = 0;
   double reconError;
   vector < vector < Point2f >> frame_mc (100);
   vector < vector < Mat >> frame_HU (100);
   vector < Mat > tempMatHU (100);
   vector < int >contornoSize;
   vector <vector<int>> globalCorr(20);
   vector <Mat> Imagenes;
   //vector <vector<double>>areasPrev(100);


   ofstream fileOut("descriptorsFrame.res");

   /*VALIDACIÓN DE PARAMETROS*/
   if (argc < 3)
   {
      cerr << "Faltan argumentos:\n\n\tUso:\n\t\t " << argv[0] << " ListaArchivos DirectorioSalida [Umbral Mínimo de Distancia] [Umbraliza]"
           << endl << endl
           << "\tListaArchivos -> Archivo de texto que contiene lista de archivos a procesar"
           << endl;
   }

   dataFiles = String(argv[1]);
   OutDir = String(argv[2]) + "/";
   if (argc > 3)
   {
      umbralDistance = atof(argv[3]);
      if (umbralDistance < 0.)
      {
         cerr << "El umbral de distancia minimo tiene que ser mayor que 0"
              << endl;
         exit(1);
      }
      if (argc>4)
      {
         umbralHu = atof(argv[4]);
         if (umbralHu < 0.)
         {
            cerr << "El umbral de Hu minimo  tiene que ser mayor que 0"
                 << endl;
            exit(1);
         }
         if (argc > 5)
            Umbraliza = false;
      }
   }


   infile.open(dataFiles);
   
   
   namedWindow ("Output", 1);
   if (Umbraliza)
      namedWindow ("Umbralizada", 1);

	//Elemento necesario para el ajuste de dilatación.
   Mat element = getStructuringElement (dilation_type,
      Size (2 * dilation_size + 1,
      2 * dilation_size + 1),
      Point (dilation_size,
      dilation_size)); 

   while (getline (infile, file))
   {
      frameData fD;
      vector<float> labels;
      istringstream iss (file);
      cout <<"file:\t"<<file<<endl;

      //Leemos el path de la imagen
      fD.fileName = file;
      fD.Image = imread (fD.fileName, IMREAD_GRAYSCALE);
      if (!fD.Image.data)
      {
         cout << "Could not open or find the image" << std::endl;
         return -1;
      }

      if (Umbraliza)
         threshold (fD.Image, fD.Image,25, 255, THRESH_BINARY_INV );
      else
         dilate (fD.Image, fD.Image, element);

      findContours (fD.Image, fD.contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

      for (unsigned int i = 0; i < fD.contours.size (); i++)
      {
         Moments mo;
         momHu mH;
         Point2f cM;

         mo = moments (fD.contours[i]);
         fD.mu.push_back(mo); 

         cM = Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                     static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));
         fD.mc.push_back(cM);

        	HuMoments (mo, mH.mH);
        	for (unsigned int j = 0; j < 7; ++j)
            mH.mH[j] = -1 * copysign (1.0,mH.mH[j]) *log10 (abs (mH.mH[j]) + 1e-8);
        	fD.momentsHu.push_back(mH);
     }
     Frames.push_back(fD);
   }
   
   for (unsigned int i = 1; i < Frames.size(); ++i)
       vector < Point3f > correspondences =  findCorrespondences2 (Frames, i-1, i, umbralHu, umbralDistance, i);

   infile.close ();
   fileOut.close();
   return 0;
}




