#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <Mosaic.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <algorithm>
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
#include <FeatureTracker.h>
#include <sortContours.h>

using namespace cv;
using namespace std;

String image_path;
String file;
RNG rng (12345);


/*!
\struct featDescriptor 
\brief Esta clase guarda contiene las características de un cuadro, con la finalidad de realizar un seguimiento múltiple a cada uno de los objetos de interés.
*/

struct featDescriptor:public trackedFeature
{
   /*!
      \var int idxFrame;
      \brief Contiene el identificador de cada cuadro
    */
   int idxFrame;
   /*!
      \var int idxFeat;
      \brief Contiene el identificador de cada objeto en cada cuadro.
    */
   int idxFeat;

   /*!
      \var vector < Point > objContour;
      \brief El contorno del objeto almacenado.
   */
   vector < Point > objContour;

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
      \fn featDescriptor()
      \brief Constructor que inicializa a idxFrame e idxFeat.
   */

   /*!
      \var float area;
      \brief Contiene el area de cada objeto en cada cuadro.
    */

   float area;

   /*!
      \var float perimetro;
      \brief Contiene el perimetro de cada objeto en cada cuadro.
    */

   float perimetro;

   featDescriptor ()
   {
      idxFrame = idxFeat = -1;
   }

   /*!
      \fn featDescriptor (const frameData & F, int iF, int io)
      \brief Constructor de inicialización, asigna valores a idxFrame, idObj, momentsHu y mc.
      \param frameData &F
      \param int iF Índice del Frame
      \param int io Índice del objeto

    */
   featDescriptor (const frameData & F, int iF, int io)
   {
      objContour = F.contours[io];
      idxFrame = iF;
      idxFeat = io;
      momentsHu = F.momentsHu[idxFeat];
      mc = F.mc[idxFeat];
      area = F.areas[idxFeat];
      perimetro = F.perimetros[idxFeat];
   }

   /*!
      \fn featDescriptor (const featDescriptor & O)
      \brief Constructor de copia. Crea un objeto a partir de otro objeto de esta misma clase. 
      \param featDescriptor &O
   */
   featDescriptor (const featDescriptor & O)
   {
      *((trackedFeature *)this) = O;
      objContour = O.objContour;
      idxFrame = O.idxFrame;
      idxFeat = O.idxFeat;
      momentsHu = O.momentsHu;
      mc = O.mc;
      area = O.area;
      perimetro = O.perimetro;
   }

   /*!
      \fn void operator = (const featDescriptor & O)
      \brief Sobre carga del operador igual (=).
      \param featDescriptor &O
   */
   void operator = (const featDescriptor & O)
   {
      *((trackedFeature *)this) = O;
      objContour.clear();
      objContour = O.objContour;
      idxFrame = O.idxFrame;
      idxFeat = O.idxFeat;
      momentsHu = O.momentsHu;
      mc = O.mc;
      area = O.area;
      perimetro = O.perimetro;
   }
   
   /*!
      \fn double Distance (const trackedFeature & o)
      \brief Función que incluye la distancia entre dos objetos con respecto a sus centros de masa.
      \param trackedFeature & o
      \return distance
   */
   double Distance (const trackedFeature & o)
   {
      featDescriptor *p = (featDescriptor *) & o;

      return sqrt(pow (p->mc.x - mc.x, 2.) + pow (p->mc.y - mc.y, 2.));
   }
   
   /*!
      \fn repr ()
      \brief Función que imprime variables para su depuración.
   */
   string repr ()
   {
      stringstream ss;
      string s;

      ss << "Obj[" << idxFrame << "][" << idxFeat << "]= [" << mc.x << ", " <<
         mc.y << "]";
      s = ss.str ();
      return s;
   }
};

struct tableView
{
   Mat mapLayer, linesLayer, imgView;
   unsigned int sz, sep, step;
   unsigned int rowsMap, colsMap;
   unsigned int rowsView, colsView;
   unsigned int nElements, seqSz;
   double scale;
   Scalar notdef, def;

   tableView (tempFeatureTable < featDescriptor > &tFeats, int s = 15)
   {
      unsigned int i, j, r, c;
      char buff[8];

      sz = s;
      sep = 3;
      nElements = tFeats.maxElements;
      seqSz = tFeats.maxSeq;
      colsMap = sz * seqSz + sep * (seqSz + 1);
      rowsMap = sz * nElements + sep * (nElements + 1);
      colsView = colsMap + 2 * sz;
      rowsView = rowsMap + 4 * sz;
      def = Scalar(255, 128);
      notdef = Scalar(24, 0, 48);
      scale = sz/15.;
      step = sz + sep;


      imgView = Mat::zeros(rowsView, colsView, CV_8UC3);
      mapLayer = imgView(Rect(2 * sz, 4 * sz, colsMap, rowsMap));
      linesLayer = Mat::zeros(rowsMap, colsMap, CV_8UC3);


      for (i = 0, r = sep; i < nElements; ++i, r += step )
      {
         snprintf(buff,8,"%03d", i);
         putText(imgView, buff, Point(0, r + 4 * sz + 12*scale) , 0 , .5*scale,\
           Scalar(128,255,128), 1, LINE_AA, false);
      }
      
      for (j=0, c = sep; j < seqSz; ++j, c += step)
      {
         char miniBuff[2];
         miniBuff[1] = 0;

         snprintf(buff,8,"%03d", j);
         for (i=0, r = sep; i < 3; ++i, r += step)
         {
            miniBuff[0] = buff[i];
            putText(imgView, miniBuff, Point(c + 2 * sz, r + 12 * scale) , 0 , 0.5 * scale,\
             Scalar(128,255,128), 1, LINE_AA, false);
         }
      }

   }

   void drawSquare(unsigned int elem, unsigned int seq, bool kind = true)
   {
      //Upper-left corner at (sep-2, sep-2)
      //width-height: (sz+2, sz+2)
      if (elem < nElements && seq < seqSz)
      {
         Rect R(sep - 2 + seq * step, sep - 2 + elem * step, sz + 4, sz + 4);
         if (kind)
            rectangle(mapLayer, R, Scalar(0, 0, 255), 2);
         else
            rectangle(mapLayer, R, Scalar(0, 0, 0), 2);
         mergeLayers();
      }
   }

   void mergeLayers()
   {
      unsigned int i, j;
      Vec3b *it1, *it2;
      Vec3b vZero(0,0,0);

      for (i=0;i<rowsMap;++i)
      {
         it1 = mapLayer.ptr<Vec3b>(i);
         it2 = linesLayer.ptr<Vec3b>(i);
         for (j=0;j<colsMap;++j,it1++, it2++)
            if (*it2 != vZero)
               *it1 = *it2;
      }
   }

   void refresh(tempFeatureTable < featDescriptor > &tFeats)
   {
      unsigned int i, j, r, c, sz2;
      Range rx, ry;
      sz2 = sz/2;
      
      for (i = 0, r = sep; i < nElements; ++i, r += step )
      {
         ry = Range(r, r+sz);
         for (j=0, c = sep; j < seqSz; ++j, c += step)
         {
            rx = Range(c, c+sz);
            if (tFeats.Table[j][i].status == DEFINED)
               mapLayer(ry, rx) = def;
            else
               mapLayer(ry, rx) = notdef;
         }
      }
      for (i = 0, r = sz2 + sep; i < nElements; ++i, r += step)
         for (j=0, c = sz2 + sep; j < seqSz; ++j, c += step)
         {
            
            if (tFeats.Table[j][i].matched)
            {

               Point P1(c,r), P2(c+sz+sep, tFeats.Table[j][i].next[0].idx*(sz+sep)+sz2+sep);
               line(linesLayer, P1, P2,Scalar(255,255,255), 2);
               circle(linesLayer, P1, 2, Scalar(192, 192, 192), -1);
               circle(linesLayer, P2, 2, Scalar(192, 192, 192), -1);
            }
         }
      mergeLayers();
   }
};


void getTracking (tempFeatureTable < featDescriptor > &tFeats, int umbralFrame,
                  int numObj);

void trackerViewer(tempFeatureTable < featDescriptor > &tFeats, vector < frameData > Frames, Point2f &pf)
{
   int idxFrame = 0, k, idxFeat=0, N, width, height;
   char val;
   unsigned int i, j;

   N = Frames.size();
   width  = Frames[0].Image.cols;
   height = Frames[0].Image.rows;

   Mosaic M(Size(width, height), 2, 2, 8, 8, CV_8UC3);

   string info0 = "";
   string info1 = "";
   vector <int> oIndexes;
   oIndexes.assign(N, -1);


   namedWindow ("Secuencia", 1);

   //Buscamos el primer objeto definido de la primera imagen
   for (j = 0; j< tFeats.maxSeq;++j)
   {
      for (i = 0; i < tFeats.maxElements && tFeats.Table[j][i].status != DEFINED;++i);
      if (i == tFeats.maxElements)
         idxFeat = -1; // No h ay elemento definido en ese frame.
      else
      {
         idxFeat = i;
         break;
      }
   }
   if (j == tFeats.maxSeq)
   {
      cerr << "There are no objects in the sequence!" << endl;
      return;
   }
   idxFrame = j;
   while(true)
   {
      Mat Img0, Img1;
      vector <vector <Point> > oneVector;
      
      cvtColor (Frames[idxFrame].Image, Img0, COLOR_GRAY2RGB);
      cvtColor (Frames[idxFrame+1].Image, Img1, COLOR_GRAY2RGB);

      //Dibuja los contornos si definidos.
      for (i=0;i<tFeats.maxElements;++i)
      {
         if (tFeats.Table[idxFrame][i].status == DEFINED)
         {
            oneVector.clear();
            oneVector.push_back(tFeats.Table[idxFrame][i].objContour);
            drawContours (Img0, oneVector, -1, Scalar (0, 0, 255));
         }
         if (idxFrame+1 < (int)tFeats.maxSeq && tFeats.Table[idxFrame+1][i].status == DEFINED)
         {
            oneVector.clear();
            oneVector.push_back(tFeats.Table[idxFrame+1][i].objContour);
            drawContours (Img1, oneVector, -1, Scalar (0, 0, 255));
         }
      }


      Mat Info0 = Mat::zeros(Img0.rows,Img0.cols,CV_8UC3);
      Mat Info1 = Mat::zeros(Img0.rows,Img0.cols,CV_8UC3);
      if (idxFeat != -1)
      {
//         cout<< "tFeats.Table[idxFrame][idxFeat].prev"<<tFeats.Table[idxFrame][idxFeat].prev<<endl;
         if(tFeats.Table[idxFrame][idxFeat].next.size())
         {
            int m, n;
            m = idxFrame;
            n = idxFeat;
            while(m > 0 && tFeats.Table[m][n].prev.size())
            {
               n = tFeats.Table[m][n].prev[0].idx;
               m--;
            }
            while(tFeats.Table[m][n].next.size())
            {
               Point p1(tFeats.Table[m][n].mc.x,tFeats.Table[m][n].mc.y);//Punto del actual
               n = tFeats.Table[m][n].next[0].idx;
               m++;
               Point p2(tFeats.Table[m][n].mc.x,tFeats.Table[m][n].mc.y);//Punto del anterior

               line(Img0,p1,p2,Scalar(255,0,255),3);
            }

         }


        if (tFeats.Table[idxFrame][idxFeat].status == DEFINED)
         {
            Point P0((int)rint(tFeats.Table[idxFrame][idxFeat].mc.x),(int)rint(tFeats.Table[idxFrame][idxFeat].mc.y)); 
            double humNorm = 0;
            for(int i = 0; i<7 ; i++){
               humNorm += tFeats.Table[idxFrame][idxFeat].momentsHu.mH[i]*tFeats.Table[idxFrame][idxFeat].momentsHu.mH[i];
            }
            //circle(Img0, P0, 5, Scalar(255,196,128), 3);

            oneVector.clear();
            oneVector.push_back(tFeats.Table[idxFrame][idxFeat].objContour);
            drawContours (Img0, oneVector, -1, Scalar (0, 255, 0));

            info0 = "Objeto "+ to_string(tFeats.Table[idxFrame][idxFeat].idxFeat)+ "/"+to_string(tFeats.featsFrame(idxFrame))+", Frame = "+to_string(tFeats.Table[idxFrame][idxFeat].idxFrame);  
            putText(Info0,info0,Point(50,50),0,.7,Scalar(255,255,128));
            info0 ="MC = [" +to_string(tFeats.Table[idxFrame][idxFeat].mc.x) +","+to_string(tFeats.Table[idxFrame][idxFeat].mc.y)+"]";  
            putText(Info0,info0,Point(50,100),0,.7,Scalar(255,255,128));
            info0 ="Momentos de Hu Normalizados = "+ to_string(sqrt(humNorm));  
            putText(Info0,info0,Point(50,150),0,.7,Scalar(255,255,128));
            info0 ="Area = " + to_string(tFeats.Table[idxFrame][idxFeat].area);  
            putText(Info0,info0,Point(50,200),0,.7,Scalar(255,255,128));
            info0 ="Perimetro = "+to_string(tFeats.Table[idxFrame][idxFeat].perimetro);  
            putText(Info0,info0,Point(50,250),0,.7,Scalar(255,255,128));

            {
               int nx, pr;

               if (tFeats.Table[idxFrame][idxFeat].next.size())
                  nx = tFeats.Table[idxFrame][idxFeat].next[0].idx;
               else
                  nx = -1;
               if (tFeats.Table[idxFrame][idxFeat].prev.size())
                  pr = tFeats.Table[idxFrame][idxFeat].prev[0].idx;
               else
                  pr = -1;
               info0 = "Next, Prev = " + to_string(nx) + ", " +  to_string(pr) ;
               putText(Info0,info0,Point(50,300),0,.7,Scalar(255,255,128));
            }

            if (idxFrame < N-2)
            {
               if (tFeats.Table[idxFrame][idxFeat].next.size())
               {
                  k = tFeats.Table[idxFrame][idxFeat].next[0].idx;
                  Point P1((int)rint(tFeats.Table[idxFrame+1][k].mc.x),(int)rint(tFeats.Table[idxFrame+1][k].mc.y)); 
                  circle(Img1, P1, 5, Scalar(196,255,128), 3);
                  oneVector.clear();
                  oneVector.push_back(tFeats.Table[idxFrame+1][k].objContour);
                  drawContours (Img1, oneVector, -1, Scalar (255, 0, 0));
                  double humNorm2 = 0;
            for(int i = 0; i<7 ; i++){
               humNorm2 += tFeats.Table[idxFrame+1][k].momentsHu.mH[i]*tFeats.Table[idxFrame+1][k].momentsHu.mH[i];
            }
                  info1 = "Objeto "+ to_string(tFeats.Table[idxFrame+1][k].idxFeat)+ ", Frame = "+to_string(tFeats.Table[idxFrame+1][k].idxFrame);  
                  putText(Info1,info1,Point(50,50),0,.7,Scalar(255,255,128));
                  info1 ="MC = [" +to_string(tFeats.Table[idxFrame+1][k].mc.x) +","+to_string(tFeats.Table[idxFrame+1][k].mc.y)+"]";  
                  putText(Info1,info1,Point(50,100),0,.7,Scalar(255,255,128));
                  info1 ="Momentos de Hu Normalizados = "+ to_string(sqrt(humNorm2));  
                  putText(Info1,info1,Point(50,150),0,.7,Scalar(255,255,128));
                  info1 ="Area = " + to_string(tFeats.Table[idxFrame+1][k].area);  
                  putText(Info1,info1,Point(50,200),0,.7,Scalar(255,255,128));
                  info1 ="Perimetro = "+to_string(tFeats.Table[idxFrame+1][k].perimetro);  
                  putText(Info1,info1,Point(50,250),0,.7,Scalar(255,255,128));

                  
                  {
                     int nx, pr;

                     if (tFeats.Table[idxFrame+1][k].next.size())
                        nx = tFeats.Table[idxFrame+1][k].next[0].idx;
                     else
                        nx = -1;
                     if (tFeats.Table[idxFrame+1][k].prev.size())
                        pr = tFeats.Table[idxFrame+1][k].prev[0].idx;
                     else
                        pr = -1;
                     info0 = "Next, Prev = " + to_string(nx) + ", " +  to_string(pr) ;
                     putText(Info1,info1,Point(50,300),0,.7,Scalar(255,255,128));
                  }
               }
            }


         }
         //drawContours (Img0, fD.contours, -1, Scalar (0, 255, 0));
      }
      






      circle (Img0, Point((int)pf.x, (int)pf.y), 5, Scalar (0, 0, 255), FILLED, LINE_8);
      circle (Img1, Point((int)pf.x, (int)pf.y), 5, Scalar (0, 0, 255), FILLED, LINE_8);
      M.setFigure(Img0, 0, 0);
      M.setFigure(Img1, 0, 1);
      M.setFigure(Info0,1,0);
      M.setFigure(Info1,1,1);
      M.show("Secuencia");



      if ((val = waitKeyEx( 0 )) ==27 )
         break;

      switch (val)
      {
         case 'w'://Retrocede el frame
            if (idxFrame > 0)
            {
               idxFrame = idxFrame - 1;
               idxFeat = oIndexes[idxFrame];
            }
            break;
         case 's'://Avanza el frame
            if (idxFrame < N-2)
            {
               oIndexes[idxFrame] = idxFeat; //Almacenamos el indice del objeto en el frame idxFrame
               if (tFeats.Table[idxFrame][idxFeat].next.size())
                  //k es ahora hacia donde nos vamos a mover.
                  k = tFeats.Table[idxFrame][idxFeat].next[0].idx;
               else
                  k = -1;
               if (k != -1 && tFeats.Table[idxFrame+1][k].status == DEFINED)
                  idxFeat = k;
               else
               {
                  unsigned int l;

                  for (l=0;l<tFeats.maxElements;++l)
                     if (tFeats.Table[idxFrame+1][l].status == DEFINED)
                        break;
                  if (l < tFeats.maxElements)
                     idxFeat = l;
                  else
                     l = 0;

               }
               idxFrame = idxFrame + 1;
            }
               break;
            case 'a'://Retrocede el objeto
               if (idxFeat >= 0)
               {
                  oIndexes[idxFrame] = idxFeat;
                  for (k = idxFeat - 1; k >= 0 && tFeats.Table[idxFrame][k].status != DEFINED;--k);
                  if (k != -1)
                     idxFeat = k;
                  else
                  {
                     for (k = tFeats.maxElements - 1; k > idxFeat && tFeats.Table[idxFrame][k].status != DEFINED;--k);
                     idxFeat = k;
                  }
               }
               break;
            case 'd'://Avanza el objeto
               if (idxFeat != -1)
               {
                  for (i = idxFeat+1; i < tFeats.maxElements && tFeats.Table[idxFrame][i].status != DEFINED;++i);
                  if (i != tFeats.maxElements)
                     idxFeat = i;
                  else
                  {
                     for (i = 0; i < (unsigned)idxFeat && tFeats.Table[idxFrame][i].status != DEFINED;++i);
                     idxFeat = i;
                  }
               }
            break;
      }
   }
   destroyWindow ("Secuencia");
}

int main (int argc, char **argv)
{
   //Files to write DATA
   String nameFile = "realPoints.m";
   ofstream realPoints (nameFile);
   //vector <vector < Point2i >> mcGlobal(100);
   Mat pointMat;
   int t;
   int dilation_type = 2;
   int dilation_size = 1;
   unsigned int maxObjs;

   double umbralDistance = 20.;
   double umbralHu = 500.;
   double umbralArea = 50.;
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
         << "\tUmbralFrame -> cantidad de cuadros a validar," << endl
         << "\tMaxObjs -> Cantidad Máxima de Objetos a seguir." << endl;
         exit(1);
   }

   dataFiles = String (argv[1]);

   int umbralFrame = atoi (argv[2]);
   maxObjs = atoi(argv[3]);
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

   tempFeatureTable < featDescriptor > tFeats (maxObjs, umbralFrame, umbralDistance);
   tableView tempTableView (tFeats, 15);

   infile.open (dataFiles);


   namedWindow ("contornos", 1);
   namedWindow ("Table View", 1);
   //  if (Umbraliza)
   // namedWindow ("Umbralizada", 1);

   //Elemento necesario para el ajuste de dilatación.
   Mat element = getStructuringElement (dilation_type,
                                        Size (2 * dilation_size + 1,
                                              2 * dilation_size + 1),
                                        Point (2,
                                               2));

   t = 0;
   while (getline (infile, file))
   {
      vector < vector < Point > >tmpContours;
      frameData fD, fDo;
      vector < double >labels;
      istringstream iss (file);
      vector < featDescriptor > objs;

      cerr << "file:\t" << file << endl;

      //Leemos el path de la imagen
      fD.fileName = file;
      fD.Image = imread (fD.fileName, IMREAD_GRAYSCALE);
      if (!fD.Image.data)
      {
         cerr << "Could not open or find the image" << std::endl;
         return -1;
      }

      // if (Umbraliza)

      // else
      //    
      // erode (fD.Image, fD.Image, element);
      //threshold (fD.Image, fD.Image, 55, 255, THRESH_BINARY); // Original de Jorge Coral.
      adaptiveThreshold(fD.Image, fD.Image, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 5, 2);

      tmpContours.clear();
      findContours (fD.Image, tmpContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
      filterSmallContours (tmpContours, umbralArea);
      sortContours (tmpContours, fD.contours);
      Mat frameRGB;
      cvtColor (fD.Image, frameRGB, COLOR_GRAY2RGB);
      drawContours (frameRGB, fD.contours, -1, Scalar (0, 255, 0));
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
         circle(frameRGB, Point((int)rint(cM.x), (int)rint(cM.y)), 5, Scalar(255,196,128),3);

         HuMoments (mo, mH.mH);
         for (j = 0; j < 7; ++j)
            mH.mH[j] =
               -1 * copysign (1.0, mH.mH[j]) * log10 (abs (mH.mH[j]) + 1e-8);
         fD.momentsHu.push_back (mH);
         fD.areas.push_back (mo.m00);
         fD.perimetros.push_back(arcLength(fD.contours[i],true));
      }
      Frames.push_back (fD);
      imshow ("contornos", frameRGB);
   
      for (i = 0; i < Frames[t].contours.size (); ++i)
      {
         featDescriptor ob (Frames[t], t, i);
         objs.push_back (ob);
      }
      tFeats.addDescriptors (objs, t);
      tempTableView.refresh(tFeats);
      imshow("Table View", tempTableView.imgView);
      waitKey (30);
      tFeats.incIdx ();
      t++;
   }
   tFeats.printGrid ();
   tempTableView.drawSquare(10,15,true);
   imshow("Table View", tempTableView.imgView);
   waitKey (0);
   tempTableView.drawSquare(10,15,false);
   imshow("Table View", tempTableView.imgView);
   waitKey (30);

   cout << " tFeats.maxSeq: " << tFeats.maxSeq << endl;
   cout << " tFeats.maxElements " << tFeats.maxElements << endl;
   //Codigo Ajuste de puntos a una linea.
   vector < Mat > linesToFit;

   //getTracking (tFeats, umbralFrame, 500);

   //for (j = 0; j < tFeats.maxElements; ++j)//Número de objetos
   realPoints << "RP = [";
   for (j = 0; j < 10; ++j)     //Número de objetos
   {
      vector < Mat > pointsToFit;
      //for (i=0;i<tFeats.maxSeq;++i)//Numero de frames
      for (i = 0; i < unsigned (umbralFrame); ++i) //Numero de frames
      {

         //fOut<<tFeats.Table[i][j].mc.x<<","<<tFeats.Table[i][j].mc.y<<endl;
         Mat pts = (Mat_ < double >(3, 1) << tFeats.Table[i][j].mc.x,
                    tFeats.Table[i][j].mc.y, 1);
         if (i < (unsigned (umbralFrame) - 1))
         {
            realPoints << "[" << tFeats.Table[i][j].mc.
               x << "," << tFeats.Table[i][j].mc.y << "," << "1],";
         }
         else
         {
            realPoints << "[" << tFeats.Table[i][j].mc.
               x << "," << tFeats.Table[i][j].mc.y << "," << "1];";
         }
         //Mat pts = (Mat_ <double> (3,1)<< xs[j],ys[j],1);
         //cout<<pts<<endl;
         pointsToFit.push_back (pts);
      }
      realPoints << endl;

      Mat L = fitLine (pointsToFit, 0.0003).t ();

      linesToFit.push_back (L);
      //cout << "L[" << j << "] = " << L.t() << endl; 
   }
   realPoints << "]";
   Mat puntoDeFuga = fitLine (linesToFit, 0.0003);
   cout << "Punto de Fuga = " << puntoDeFuga << endl;
   int ptx, pty;
   ptx = puntoDeFuga.at < double >(0, 0) / puntoDeFuga.at < double >(0, 2);
   pty = puntoDeFuga.at < double >(0, 1) / puntoDeFuga.at < double >(0, 2);
   cout << "Punto de Fuga en cordenadas de la imagen: (" << ptx << "," << pty
      << ")" << endl;

   Point2f pf = Point2f(ptx,pty);
   trackerViewer(tFeats, Frames, pf);

   infile.close ();
   fileOut.close ();
   realPoints.close ();
   return 0;
}

float matchContours(featDescriptor &a, featDescriptor &b)
{
   vector< Point >::iterator iteA, endA, iteB, endB;
   double dist, d, min;
   
   iteA = a.objContour.begin();
   endA = a.objContour.end();
   iteB = b.objContour.begin();
   endB = b.objContour.end();

   dist = 0;
   for (;iteA != endA;++iteA)
   {
      
      iteB = b.objContour.begin();
      min = d = sqrt(pow(iteA->x - iteB->x, 2) + pow(iteA->y - iteB->y, 2));
      for (iteB++;iteB != endB;++iteB)
      {
         d = sqrt(pow(iteA->x - iteB->x, 2) + pow(iteA->y - iteB->y, 2));
         if (d < min)
            min = d;
      }
      dist += min;
   }
   dist /= a.objContour.size();
//   cout << "idxFrameA, idxFrameB, a.size(), b.size(), dist, distCM = {" 
//        << a.idxFrame << ", " << b.idxFrame << ", "
//        << a.objContour.size()  << ", " << b.objContour.size()  << ", "
//        << dist << ", " << sqrt(pow(a.mc.x-b.mc.x,2)+pow(a.mc.y-b.mc.y,2))
//        << "}" << endl;
   return dist;
}


void getTracking (tempFeatureTable < featDescriptor > &tFeats, int umbralFrame,
                  int numObj)
{
   ofstream tracking ("tracking.m");
   int k = 0, k_old;
   int l = 0, l_old;
   int n = 0;
   int nObjs = tFeats.maxSeq * tFeats.maxElements;
   vector < vector < Point2f > > cola (nObjs);
   vector < vector < double > > match (nObjs);

   umbralFrame = tFeats.maxSeq;
   numObj = tFeats.maxElements;
   
   int flag[umbralFrame][numObj];

   //Inicializamos el arreglo flag
   for (int i = 0; i < umbralFrame; ++i)
      memset (flag[i], 0, numObj * sizeof(int));

   for (int i = 0; i < numObj; ++i) //Número de objetos
   {
      for (int j = 0; j < umbralFrame; ++j)
      {
         if (tFeats.Table[j][i].status == DEFINED && flag[j][i] != -1)
         {
            k_old = i;
            l_old = j;
            if (tFeats.Table[j][i].next.size())
               k = tFeats.Table[j][i].next[0].idx;
            else
               k = -1;
            l = j + 1;
            cout << endl 
                 << "Processing Cell (" << i << ", " << j << ")"
                 << endl;
            cout.flush();
            while (k >=0 && tFeats.Table[l][k].status == DEFINED && l < umbralFrame)
            {
               //Calculamos el Rank Match
               match[n].push_back(matchContours(tFeats.Table[l_old][k_old], tFeats.Table[l][k]));
               //Metemos el elemento definido en el vector n de la cola.
               cola[n].push_back (Point2f (l, k));
               //Marcamos que el elemento k,l ya fue asociado.
               flag[l][k] = -1;


               //Encontramos en el renglon siguiente el correspondiente.
               l_old = l;
               k_old = k;
               if (tFeats.Table[l][k].next.size())
                  k = tFeats.Table[l][k].next[0].idx;
               else
                  k = -1;
               l++;
            }
         }
         n++;
      }
   }

   unsigned int idx = 0;
   for (unsigned int i = 0; i < cola.size(); ++i)
   {
      if(cola[i].size () < 4)
         continue;
      //cout << "Analizando objeto en la columna " << i << endl;

      tracking<<"Obj"+to_string(idx++)+"=[";
      for (unsigned int j = 0; j < cola[i].size (); j++)
      {

         int r, c;

         // i es el indice de la columna de la tabla.
         // j es el indice del renglon de la tabla
         r = cola[i][j].x;
         c = cola[i][j].y;
         //cout << tFeats.Table[r][c].idxFrame << endl;

         tracking << tFeats.Table[r][c].mc.x << ","
                  << tFeats.Table[r][c].mc.y << ","
                  << tFeats.Table[r][c].idxFrame << ","
                  << match[i][j] << ","
                  << tFeats.Table[r][c].objContour.size() << "; "
                  << endl;
         
      }
      tracking << "];" << endl;
      
   }
   tracking.close();
}









//  int t = 0;
//    vector<int> registro;
//    vector<Point2f> objeto;
//    unsigned int i, j=0;
//    vector < vector<Point2f> > tracking;
//    //int checker[tFeats.maxElements][tFeats.maxSeq];
//    Mat checker = Mat::ones(tFeats.maxSeq,tFeats.maxElements,CV_8U);
//    //Table[i][j].status
//    for (i=0;i<tFeats.maxElements;++i)
//    {
//       cout<<"iteracion i "<<i<<endl;
//          if(checker.at<uchar>(i,j)!=0)
//          {  
//             while(j<tFeats.maxSeq)
//             {
//                if(tFeats.Table[j][i].status!=MISSING)
//                {
//                   objeto.push_back(Point2f(tFeats.Table[j][i].mc.x,tFeats.Table[j][i].mc.y));
//                }else{
//                   //cout<<objeto<<endl;
//                   registro.push_back(t);
//                   tracking.push_back(objeto);
//                   objeto.clear();
//                   t = j;
//                }
//                checker.at<uchar>(i,j) = 0;
//                j++;
//             }
//             j=0;
//          }else{
//             continue;
//       }

//    }
//    return tracking;
