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
#include <frameData.h>

using namespace cv;
using namespace std;

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


vector < Point3f > findCorrespondences2 (vector < frameData > Frames,
                                         int idx1, int idx2, double umbralHu =
                                         500, double umbralDistance =
                                         60., int fIdx = 0)
{
   int Size1 = Frames[idx1].mc.size ();
   int Size2 = Frames[idx2].mc.size ();
   int index1 = 0;
   int index2 = 0;
   vector < Point3f > correspondences; //[Size1];
   double valor_min = 0, minHu = 0;
   ofstream fDist;
   double tempDistMC = 0, distance = 0;
   double finalDCD, finalHUD;
   Mat Im1, Im2, ImD1, ImD2;

   namedWindow ("Figura1", 1);
   namedWindow ("Figura2", 1);

   cvtColor (Frames[idx1].Image, Im1, COLOR_GRAY2RGB);
   cvtColor (Frames[idx2].Image, Im2, COLOR_GRAY2RGB);
   char buff[256];
   //Para evitar calcula distancia al cuadrado.
   umbralDistance *= umbralDistance;
   umbralHu *= umbralHu;

   if (fIdx == 1)
      fDist.open ("Distancias.txt");
   else
      fDist.open ("Distancias.txt", std::ios_base::app);

   fDist <<
      "\% frameIdx sizeOld, sizeNew, idxOld, idxNew, MomemtsOld MomentsNew, DistMoments_ij DistCentro_ij"
      << endl;
   for (int i = 0; i < Size1; i++)
   {
      valor_min = -1.;
      minHu = -1;
      for (int j = 0; j < Size2; j++)
      {

         distance = 0;

         fDist << fIdx << ", " << Size1 << "," << Size2 << ", "
            << i << "," << j << ", ";

         for (int k = 0; k < 7; k++)
            fDist << Frames[idx1].momentsHu[i].mH[k] << ",";
         fDist << " ";

         for (int k = 0; k < 7; k++)
            fDist << Frames[idx2].momentsHu[j].mH[k] << ",";
         fDist << " ";

         //Distancia Euclideana entre cada fila de momentos de HU.
         distance = 0;
         for (int k = 0; k < 7; k++)
            distance +=
               pow ((Frames[idx1].momentsHu[i].mH[k] -
                     Frames[idx2].momentsHu[j].mH[k]), 2.);

         //fDist << mc_old[i].x << "," << mc_old[i].y << ", " << mc_new[j].x
         //      << "," << mc_new[j].y << ", " << distance << ",";

         tempDistMC =
            pow (Frames[idx1].mc[i].x - Frames[idx2].mc[j].x,
                 2.) + pow (Frames[idx1].mc[i].y - Frames[idx2].mc[j].y, 2.);
         ImD1 = Im1.clone ();
         ImD2 = Im2.clone ();
        /* 
            circle(ImD1, Point(Frames[idx1].mc[i].x,Frames[idx1].mc[i].y), 10, Scalar(255, 0, 255),FILLED,LINE_8);
            circle(ImD2, Point(Frames[idx2].mc[j].x,Frames[idx2].mc[j].y), 10, Scalar(128, 255, 128),FILLED,LINE_8);
            snprintf(buff, 255, "dHu = %f, dEucSq = %f", sqrt(distance), sqrt(tempDistMC));
            putText(ImD1, String(buff), Point2i (50, ImD1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
            imshow("Figura1", ImD1);
            imshow("Figura2", ImD2);
            waitKey(0);
            if (waitKeyEx (30) > 0)
            break;       
          */
 //        cout << "DistE ("<< i << ", " << j << ") = " << sqrt(tempDistMC) << " | "
 //             << "DistH ("<< i << ", " << j << ") = " << sqrt(distance) << endl;
               
 //        if (tempDistMC < umbralDistance && distance < umbralHu)
         {

            //Incorporamos a la distancia euclideana los centro de masa.

            // fDist << sqrt(tempDistMC) << ", " << sqrt(distance)  << ","<<labels[i]<<endl;
            //cout<<"Area del objeto: "<<areaObjects[i]<<endl;
            //cout<<i<<" - "<<j<<"  tempDistMC   ::    "<<sqrt(tempDistMC)<<endl;
            // cout<<"frameIds "<<fIdx<<" distance: "<<distance<<" umbralHu: "<<umbralHu<<" j "<<j<<endl; 
            if (j == 0)
            {

               valor_min = tempDistMC;
               minHu = distance;
               index1 = i;
               index2 = j;
               continue;
            }
            else
            {
               if (tempDistMC< valor_min)
               {
                  //cout<<"frameIds "<<fIdx<<" distance: "<<distance<<" umbralHu: "<<umbralHu<<endl;
                  valor_min = tempDistMC;
                  index1 = i;
                  index2 = j;
                  finalDCD = tempDistMC;
                  finalHUD = distance;
               }
               if (distance < minHu)
                  minHu = distance;
            }
         }                      //if(distance < ...
      }                         //for (j=...
//      cout << "MinHu = " << sqrt(minHu) << endl << endl;
      if (valor_min >= 0)
      {
         
            circle(ImD1, Point(Frames[idx1].mc[index1].x,Frames[idx1].mc[index1].y), 10, Scalar(255, 0, 255),FILLED,LINE_8);
            circle(ImD2, Point(Frames[idx2].mc[index2].x,Frames[idx2].mc[index2].y), 10, Scalar(128, 255, 128),FILLED,LINE_8);
            snprintf(buff, 255, "dHu = %f, dEucSq = %f", sqrt(finalHUD), sqrt(finalDCD));
            putText(ImD1, String(buff), Point2i (50, ImD1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
            imshow("Figura1", ImD1);
            imshow("Figura2", ImD2);
            waitKey(0);
            if (waitKeyEx (30) > 0)
            break; 
          
         //cout << "valor_min = " << valor_min << endl;
         correspondences.push_back (Point3f (index1, index2, 1));
         index1 = index2 = 0;
      }
   }
   fDist.close ();
   destroyWindow ("Figura1");
   destroyWindow ("Figura2");
   return correspondences;
}

//Regresa una pendiente, y un punto (x0,y0) perteneciente a la recta.
Point3f getLine (vector < Point2f > points)
{

   Point3f params;
   Mat line;
   double m;
   double abscisa;
   double ordenada;
   if (points.size () == 0)
   {
      m = 0;
      abscisa = 0;
      ordenada = 0;
   }
   else
   {
      fitLine (Mat (points), line, DIST_L2, 0, 0.01, 0.01);
      m = line.at < float >(1, 0) / line.at < float >(0, 0);   //Pendiente de la recta.
      abscisa = line.at < float >(2, 0);  // valor x0 de la recta ajustada.
      ordenada = line.at < float >(3, 0); // valor y0  de la recta ajustada.
   }
   params = Point3f (m, abscisa, ordenada);
   return params;
}

//Obtenemos los coeficientes de la forma general de la recta
//Ax+By+C = 0 en su versión homogenea.
Mat getCoeffLine (Point3f params)
{
   double A, B, C;
   if (params == Point3f (0, 0, 0))
   {
      A = B = C = 0;
   }
   else
   {
      A = -params.x;
      B = 1.0;
      C = -params.z + params.y * params.x;
   }

   Mat coeff = (Mat_ < double >(4, 1) << A, B, C, 1);
   return coeff;
}
