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
#include<fftw3.h>
#include <FourierDescriptor.h>
#include <frameData.h>
#include "deepFunctions2.h"

using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);


struct centros
{
   Point2f P;
   unsigned int idx;
};

int cmpCentros(const void *a, const void *b)
{
   centros *A, *B;

   A = (centros *)a;
   B = (centros *)b;
   if (A->P.x < B->P.x)
      return -1;
   else
   {
      if (A->P.x > B->P.x)
         return 1;
      else
      {
         if (A->P.y < B->P.y)
            return -1;
         else
            if (A->P.y > B->P.y)
               return 1;
      }
   }   
   return 0;
}



int main (int argc, char **argv)
{
   //vector <vector < Point2i >> mcGlobal(100);
   Mat pointMat;
   int t = 0;
   int dilation_type = 2;
   int dilation_size = 2;
   bool Umbraliza = true;
   double umbralDistance = 60.;
   double umbralHu= 500.;
   double umbralArea = 50;
   vector<frameData> Frames;
   unsigned int h, i, j;

  
   String dataFiles;
   ifstream infile;
   Mat image;

   vector < vector < Point2f >> frame_mc (100);
   vector < vector < Mat >> frame_HU (100);
   vector < Mat > tempMatHU (100);
   vector < int >contornoSize;
   vector <vector<int>> globalCorr(20);
   vector <Mat> Imagenes;
   //vector <vector<double>>areasPrev(100);


   ofstream fileOut("descriptorsFrame.res");

   /*VALIDACIÓN DE PARAMETROS*/
   if (argc < 4)
   {
      cerr << "Faltan argumentos:\n\n\tUso:\n\t\t " << argv[0] << "ListaArchivos"<<" [umbralArea]"<< "[umbralDistance]"<<" [umbralHu]"
           << endl << endl
           << "\tListaArchivos -> Archivo de texto que contiene lista de archivos a procesar"
           << endl;
   }

   dataFiles = String(argv[1]);

   if (argc > 3)
   {
      umbralArea = atof(argv[3]);
      if (umbralArea < 0.)
      {
         cerr << "El umbral de área minimo tiene que ser mayor que 0"
              << endl;
         exit(1);
      }
      if (argc>4)
      {
         umbralDistance = atof(argv[4]);
         if (umbralDistance < 0.)
         {
            cerr << "El umbral de distancia minimo tiene que ser mayor que 0"
                 << endl;
            exit(1);
         }
         if (argc>5)
         {
            umbralHu = atof(argv[5]);
            if (umbralHu < 0.)
            {
               cerr << "El umbral de Hu minimo  tiene que ser mayor que 0"
                    << endl;
               exit(1);
            }
         }
      }
   }
 

   infile.open(dataFiles);
   
   
 //  namedWindow ("Output", 1);
 //  if (Umbraliza)
     // namedWindow ("Umbralizada", 1);

	//Elemento necesario para el ajuste de dilatación.
   Mat element = getStructuringElement (dilation_type,
      Size (2 * dilation_size + 1,
      2 * dilation_size + 1),
      Point (dilation_size,
      dilation_size)); 

   while (getline (infile, file))
   {
      vector < vector < Point > >contours;
      frameData fD, fDo;
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
/*
      {
         Moments mo;
         centros *C;
         Point2f cM;

         C = new centros[contours.size ()];
         for (i = 0; i < contours.size ();++i)
         {
            mo = moments (contours[i]);
            if (mo.m00 < umbralArea)
               continue;
            cM = Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                     static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));


            C[i].P = cM;
            C[i].idx = i;
         }

         qsort (C, contours.size (), sizeof(centros), cmpCentros);
         fD.contours = vector < vector < Point > >(contours.size());
         cout<< "contours.size = " << contours.size()<< endl;
         cout<< "fD.contours.size = " << fD.contours.size()<< endl;

         for (i = 0; i < contours.size ();++i)
         {
            vector <Point> P;
            for (j = 0; j < contours[i].size ();++j)
            {
               cout << "C[" << i << "].idx = " << C[i].idx<< ", j = " << j << endl;
               P.push_back(contours[C[i].idx][j]);
            }
            fD.contours.push_back(P);
         }
         contours.clear();

         delete[] C;
       }
*/
      for (i = 0; i < fD.contours.size (); i++)
      {
         Moments mo;
         momHu mH;
         Point2f cM;

         mo = moments (fD.contours[i]);
         if (mo.m00 < umbralArea)
            continue;

         fD.mu.push_back(mo); 

         cM = Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                     static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));
         fD.mc.push_back(cM);

        	HuMoments (mo, mH.mH);
        	for (j = 0; j < 7; ++j)
            mH.mH[j] = -1 * copysign (1.0,mH.mH[j]) *log10 (abs (mH.mH[j]) + 1e-8);
        	fD.momentsHu.push_back(mH);
          fD.areas.push_back(mo.m00);
     }
   
    

     t++;
     Frames.push_back(fD);
   }
   vector <vector<Point2f>> mcCorrespondences(100);
   vector <vector < Point3f >> correspondences;


   /*Se generan filas de correspondencias para cada objeto encontrado en cada uno de los frames.*/
   for (i = 1; i < Frames.size(); ++i)
   {
       correspondences.push_back(findCorrespondences2 (Frames, i-1, i, umbralHu, umbralDistance, i));
       cout<<"{" << i << "}" <<correspondences.back()<<endl;
   }
   
   //Inicializamos cada Vector de indices con la el indice de la primera linea.
   vector<vector<int> > idxLines(correspondences[0].size());
   vector<vector<Point2f> > ptsLines(correspondences[0].size());
   //cout<<"correspondences[0].size()+1: " <<correspondences[0].size()+2<<endl;
   for (i=0;i<correspondences[0].size(); ++i)
      idxLines[i].push_back(correspondences[0][i].x);

   //Añadimos el indice correspondiente a la segunda linea.
   for (i=0;i<correspondences[0].size(); ++i)
      idxLines[i].push_back(correspondences[0][i].y);
   
   
   for (h = 0; h < correspondences.size()-1; ++h)
   {
      //Añadimos el indice correspondiente a la cuarta linea.
      //cout<<correspondences.size()<<endl;
      for (i=0;i<correspondences[h].size(); ++i)
      {

         int idx = idxLines[i][h+1];

         if (idx == -1)
            continue;

         //Buscamos la correspondencia
         for (j=0; j<correspondences[h+1].size(); ++j)
            if (correspondences[h+1][j].x == idx)
               break;

        //cout<<"[ "<<idxLines.size()<<" , " <<correspondences[h].size()<<" ]"<<endl;
        //cout<<"h: "<<h<<" i: "<<i<<endl;
        //cout<<" idxLines[i][h+1]: "<<idxLines[i][h+1]<<endl;
         if (j < correspondences[h+1].size())//Validamos que lo haya encontrado.
            idxLines[i].push_back(correspondences[h+1][j].y);
         else
            idxLines[i].push_back(-1);//Marcamos que hasta ahí llego la linea.
         cout<<idxLines[i][h]<<endl;      
      }

   }

   for (i=0;i<idxLines.size(); ++i)
   {
      cout << "Indices de la linea "<< i << " : "; 
      for (j=0;j < idxLines[i].size()-1; ++j)
        ptsLines[i].push_back(Point2f(Frames[j].mc[idxLines[i][j]]));
     // cout << idxLines[i][j] << endl;
   }
   
   infile.close ();
   fileOut.close();
   return 0;
}




