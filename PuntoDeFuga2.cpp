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
#include <frameData.h>
#include "deepFunctions2.h"
#include <ObjTracker.h>
#include <sortContours.h>

using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);

struct objDescriptor:public trackedObj
{
   int idxFrame, idxObj;
   momHu momentsHu;
   Point2f mc;
     objDescriptor ()
   {
      idxFrame = idxObj = -1;
   }
   objDescriptor (const frameData & F, int iF, int io)
   {
      idxFrame = iF;
      idxObj = io;
      momentsHu = F.momentsHu[idxObj];
      mc = F.mc[idxObj];
   }
   objDescriptor (const objDescriptor & O)
   {
      idxFrame = O.idxFrame;
      idxObj = O.idxObj;
      momentsHu = O.momentsHu;
      mc = O.mc;
   }

   void operator = (const objDescriptor & O)
   {
      idxFrame = O.idxFrame;
      idxObj = O.idxObj;
      momentsHu = O.momentsHu;
      mc = O.mc;
   }
   float Distance (const trackedObj & o)
   {
      objDescriptor *p = (objDescriptor *) & o;

      return pow (p->mc.x - mc.x, 2.) + pow (p->mc.y - mc.y, 2.);
   }

   string repr ()
   {
      stringstream ss;
      string s;

      ss << "Obj[" << idxFrame << "][" << idxObj << "]= [" << mc.
         x << ", " << mc.y << "]";
      s = ss.str ();
      return s;
   }
};

int main (int argc, char **argv)
{
   //vector <vector < Point2i >> mcGlobal(100);
   Mat pointMat;
   int t;
   int dilation_type = 2;
   int dilation_size = 2;
   bool Umbraliza = true;
   double umbralDistance = 30.;
   double umbralHu = 500.;
   double umbralArea = 50;
   vector < frameData > Frames;
   unsigned int i, j;


   String dataFiles;
   ifstream infile;
   Mat image;

   vector < vector < Point2f >> frame_mc (100);
   vector < vector < Mat >> frame_HU (100);
   vector < Mat > tempMatHU (100);
   vector < int >contornoSize;
   vector < vector < int >>globalCorr (20);
   vector < Mat > Imagenes;
   //vector <vector<double>>areasPrev(100);


   ofstream fileOut ("descriptorsFrame.res");

   /*VALIDACIÓN DE PARAMETROS */
   if (argc < 4)
   {
      cerr << "Faltan argumentos:\n\n\tUso:\n\t\t " << argv[0] <<
         "ListaArchivos" << " [umbralArea]" << "[umbralDistance]" <<
         " [umbralHu]" << endl << endl <<
         "\tListaArchivos -> Archivo de texto que contiene lista de archivos a procesar"
         << endl;
   }

   dataFiles = String (argv[1]);

   if (argc > 2)
   {
      umbralArea = atof (argv[2]);
      if (umbralArea < 0.)
      {
         cerr << "El umbral de área minimo tiene que ser mayor que 0"
            << endl;
         exit (1);
      }
      if (argc > 3)
      {
         umbralDistance = atof (argv[3]);
         if (umbralDistance < 0.)
         {
            cerr << "El umbral de distancia minimo tiene que ser mayor que 0"
               << endl;
            exit (1);
         }
         if (argc > 4)
         {
            umbralHu = atof (argv[4]);
            if (umbralHu < 0.)
            {
               cerr << "El umbral de Hu minimo  tiene que ser mayor que 0"
                  << endl;
               exit (1);
            }
         }
      }
   }

   cerr << "Umbral Area: " << umbralArea << endl;
   cerr << "Umbral Distancia: " << umbralDistance << endl;
   cerr << "Umbral Hu: " << umbralHu << endl;

   temporalObjsMem < objDescriptor > tObjs (20, 20, 10, pow(umbralDistance, 2));

   infile.open (dataFiles);


   //  namedWindow ("Output", 1);
   //  if (Umbraliza)
   // namedWindow ("Umbralizada", 1);

   //Elemento necesario para el ajuste de dilatación.
   Mat element = getStructuringElement (dilation_type,
                                        Size (2 * dilation_size + 1,
                                              2 * dilation_size + 1),
                                        Point (dilation_size,
                                               dilation_size));

   t = 0;
   while (getline (infile, file))
   {
      vector < vector < Point > > tmpContours;
      frameData fD, fDo;
      vector < float >labels;
      istringstream iss (file);
      vector < objDescriptor > objs;

      cerr << "file:\t" << file << endl;

      //Leemos el path de la imagen
      fD.fileName = file;
      fD.Image = imread (fD.fileName, IMREAD_GRAYSCALE);
      if (!fD.Image.data)
      {
         cerr << "Could not open or find the image" << std::endl;
         return -1;
      }

      if (Umbraliza)
         threshold (fD.Image, fD.Image, 25, 255, THRESH_BINARY_INV);
      else
         dilate (fD.Image, fD.Image, element);

      findContours (fD.Image, fD.contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
      filterSmallContours(tmpContours, umbralArea);
      sortContours (tmpContours, fD.contours);

      for (i = 0; i < fD.contours.size (); i++)
      {
         Moments mo;
         momHu mH;
         Point2f cM;

         mo = moments (fD.contours[i]);
         if (mo.m00 < umbralArea)
            continue;

         fD.mu.push_back (mo);

         cM = Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                       static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));
         fD.mc.push_back (cM);

         HuMoments (mo, mH.mH);
         for (j = 0; j < 7; ++j)
            mH.mH[j] =
               -1 * copysign (1.0, mH.mH[j]) * log10 (abs (mH.mH[j]) + 1e-8);
         fD.momentsHu.push_back (mH);
         fD.areas.push_back (mo.m00);
      }
      Frames.push_back (fD);

      for (i = 0; i < Frames[t].contours.size (); ++i)
      {
         objDescriptor ob (Frames[t], t, i);
         objs.push_back (ob);
      }
      tObjs.addDescriptors (objs, t);
      tObjs.printGrid();
      tObjs.incIdx ();
      t++;
   }

   ofstream fOut,fOut2;
   fOut.open("tracking.txt");
   fOut2.open("tracking2.py");
   cout << "T = [";
   unsigned int k = 0;
   cout<<" tObjs.maxSeq: "<<tObjs.maxSeq<<endl;
   cout<<" tObjs.maxElements "<<tObjs.maxElements<<endl;

//   double xs[] = {400,350,250,200,150};
  // double ys[] = {400,250,190,150,100};
   for (i=0;i<tObjs.maxSeq;++i)//Numero de frames
   {
      vector<Mat> pointsToFit;
      for (j=0;j<tObjs.maxElements;++j)//Número de objetos
      {
      fOut<<tObjs.Table[j][i].mc.x<<","<<tObjs.Table[j][i].mc.y<<endl;
      Mat pts = (Mat_ <double> (3,1)<< tObjs.Table[j][i].mc.x,tObjs.Table[j][i].mc.y,1);
      //Mat pts = (Mat_ <double> (3,1)<< xs[j],ys[j],1);
      pointsToFit.push_back(pts);
      }

      //cout<<pointsToFit[0].t()<<endl;
      cout<<fitLine(pointsToFit)<<endl;

   }

   //Codigo Ajuste de puntos a una linea.
   

   fOut2.close();
   fOut.close();
   infile.close ();
   fileOut.close ();
   return 0;
}

/*
   for (i=0;i<tObjs.maxSeq;++i)
   {
    fOut2<<"[";
    for (j=0;j<tObjs.maxElements;++j)
    {
      fOut<<tObjs.Table[i][j].mc.x<<","<<tObjs.Table[i][j].mc.y<<endl; 
     if (tObjs.Table[i][j].status == DEFINED)
         cout << tObjs.Table[i][j].mc.x << ", "
              << tObjs.Table[i][j].mc.y << ", ";
      else
         cout << "0, 0, ";
    }
    }  


    if (tObjs.Table[i][j].status == DEFINED){
      cout << tObjs.Table[i][j].mc.x << ", "
           << tObjs.Table[i][j].mc.y << ";" << endl;
    }
   else{
      cout << "0, 0;" << endl;
   }
   }

   */



/*for (j=0;j<tObjs.maxElements-1;++j)//Número de objetos
{

   fOut<<tObjs.Table[0][j].mc.x<<","<<tObjs.Table[0][j].mc.y<<","<<
      tObjs.Table[1][j].mc.x<<","<<tObjs.Table[1][j].mc.y<<","<<
      tObjs.Table[2][j].mc.x<<","<<tObjs.Table[2][j].mc.y<<","<<
      tObjs.Table[3][j].mc.x<<","<<tObjs.Table[3][j].mc.y<<","<<
      tObjs.Table[4][j].mc.x<<","<<tObjs.Table[4][j].mc.y<<","<<
      tObjs.Table[5][j].mc.x<<","<<tObjs.Table[5][j].mc.y<<","<<
      tObjs.Table[6][j].mc.x<<","<<tObjs.Table[6][j].mc.y<<","<<
      tObjs.Table[7][j].mc.x<<","<<tObjs.Table[7][j].mc.y<<","<<
      tObjs.Table[8][j].mc.x<<","<<tObjs.Table[8][j].mc.y<<","<<
      tObjs.Table[9][j].mc.x<<","<<tObjs.Table[9][j].mc.y<<","<<
      tObjs.Table[10][j].mc.x<<","<<tObjs.Table[10][j].mc.y<<","<<
      tObjs.Table[11][j].mc.x<<","<<tObjs.Table[11][j].mc.y<<","<<endl;

      for(i=0;i<tObjs.maxSeq-1;++i){
   fOut<<"mc"+  to_string(i)+",";
}
fOut<<"mc"+  to_string(i)<<endl;


}*/