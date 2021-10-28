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
#include <deepFunctions2.h>
#include <ObjTracker.h>
#include <sortContours.h>

using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);


/*!
\struct objDescriptor 
\brief Esta clase guarda contiene las características de un cuadro, con la finalidad de realizar un seguimiento múltiple a cada uno de los objetos de interés.
*/

struct objDescriptor:public trackedObj
{
   /*!
   \var int idxFrame;
   \brief Contiene el identificador de cada cuadro
   */
   int idxFrame;
    /*!
   \var int idxObj;
   \brief Contiene el identificador de cada objeto en cada cuadro.
   */
   int idxObj;
    /*!
   \var momHu momentsHu;
   \brief Variable de tipo momHu que contiene a los 7 valores que representan a los momentos de Hu..
   */
   momHu momentsHu;
   /*!
   \var Point2f mc;
   \brief Contiene al centro de masa de cada objeto en coordenadas de la imagen.
   */
   Point2f mc;

   /*!
   \fn objDescriptor()
   \brief Constructor que inicializa a idxFrame e idxObj.
   */
     objDescriptor ()
   {
      idxFrame = idxObj = -1;
   }
   /*!
      \fn objDescriptor (const frameData & F, int iF, int io)
      \brief Constructor de inicialización, asigna valores a idxFrame, idObj, momentsHu y mc.
      \param frameData &F
      \param int iF
      \param int io
      
   */
   objDescriptor (const frameData & F, int iF, int io)
   {
      idxFrame = iF;
      idxObj = io;
      momentsHu = F.momentsHu[idxObj];
      mc = F.mc[idxObj];
   }

   /*!
      \fn objDescriptor (const objDescriptor & O)
      \brief Constructor de copia. Crea un objeto a partir de otro objeto de esta misma clase. 
      \param objDescriptor &O
   */
   objDescriptor (const objDescriptor & O)
   {
      idxFrame = O.idxFrame;
      idxObj = O.idxObj;
      momentsHu = O.momentsHu;
      mc = O.mc;
   }
   /*!
      \fn void operator = (const objDescriptor & O)
      \brief Sobre carga del operador igual (=).
      \param objDescriptor &O
   */
   void operator = (const objDescriptor & O)
   {
      idxFrame = O.idxFrame;
      idxObj = O.idxObj;
      momentsHu = O.momentsHu;
      mc = O.mc;
   }
    /*!
      \fn double Distance (const trackedObj & o)
      \brief Función que incluye la distancia entre dos objetos con respecto a sus centros de masa.
      \param trackedObj & o
      \return distance
    */   
   double Distance (const trackedObj & o)
   {
      objDescriptor *p = (objDescriptor *) & o;

      return pow (p->mc.x - mc.x, 2.) + pow (p->mc.y - mc.y, 2.);
   }
   /*!
      \fn repr ()
      \brief Función que imprime variables para su depuración.
    */  
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
   //Files to write DATA
   String nameFile = "realPoints.m";
   ofstream realPoints(nameFile);
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
   if (argc < 3)
   {
      cerr << "Faltan argumentos:\n\n\tUso:\n\t\t " << argv[0] <<
         "ListaArchivos" << "UmbralFrame" << endl << endl <<
         "\tListaArchivos -> Archivo de texto que contiene lista de archivos a procesar"
         <<"\tUmbralFrame -> cantidad de cuadros a validar." 
         <<endl;
   }

   dataFiles = String (argv[1]);

   int umbralFrame = atoi(argv[2]);
/*
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
   */

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
      vector < double >labels;
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

         cM = Point2f (static_cast < double >(mo.m10 / (mo.m00 + 1e-5)),
                       static_cast < double >(mo.m01 / (mo.m00 + 1e-5)));
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

   cout<<" tObjs.maxSeq: "<<tObjs.maxSeq<<endl;
   cout<<" tObjs.maxElements "<<tObjs.maxElements<<endl;
   //Codigo Ajuste de puntos a una linea.
   vector<Mat> linesToFit;


   //for (j = 0; j < tObjs.maxElements; ++j)//Número de objetos
   realPoints<<"RP = [";
   for (j = 0; j < 18; ++j)//Número de objetos
   {
      vector<Mat> pointsToFit;
      //for (i=0;i<tObjs.maxSeq;++i)//Numero de frames
      for (i = 0; i < unsigned(umbralFrame); ++i)//Numero de frames
      {
         //fOut<<tObjs.Table[i][j].mc.x<<","<<tObjs.Table[i][j].mc.y<<endl;
         Mat pts = (Mat_ <double> (3,1)<< tObjs.Table[i][j].mc.x,tObjs.Table[i][j].mc.y,1);
         if(i<(unsigned(umbralFrame)-1)){
            realPoints<<"["<<tObjs.Table[i][j].mc.x<<","<<tObjs.Table[i][j].mc.y<<","<<"1],";
         }else{
            realPoints<<"["<<tObjs.Table[i][j].mc.x<<","<<tObjs.Table[i][j].mc.y<<","<<"1];";
         }
         //Mat pts = (Mat_ <double> (3,1)<< xs[j],ys[j],1);
         cout<<pts<<endl;
         pointsToFit.push_back(pts);
      }
      realPoints<<endl;

      Mat L = fitLine(pointsToFit, 0.0003).t();
      
      linesToFit.push_back(L);
      cout << "L[" << j << "] = " << L.t() << endl; 
   }
   realPoints<<"]";
   Mat puntoDeFuga = fitLine(linesToFit, 0.0003);
   cout << "Punto de Fuga = " << puntoDeFuga<< endl;
   int ptx,pty;
   ptx = puntoDeFuga.at<double>(0,0)/puntoDeFuga.at<double>(0,2);
   pty = puntoDeFuga.at<double>(0,1)/puntoDeFuga.at<double>(0,2);
   cout<<"Punto de Fuga en cordenadas de la imagen: ("<<ptx<<","<<pty<<")"<<endl;
   Point2f pf = Point2f(ptx,pty);
   Mat outImage;
   cvtColor (Frames[0].Image,outImage, COLOR_GRAY2RGB);
   namedWindow ("pf", 1);
   circle(outImage,Point(ptx,pty),5,Scalar(0,0,255),FILLED,LINE_8);
   imshow("pf",outImage);
   waitKey(0);
   destroyWindow ("pf");
   infile.close ();
   fileOut.close ();
   realPoints.close();
   return 0;
}

