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


Mat cambia (Mat M)              
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
   Mat Im1, Im2, ImD1, ImD2;

   namedWindow ("Figura1", 1);
   namedWindow ("Figura2", 1);

   cvtColor (Frames[idx1].Image, Im1, COLOR_GRAY2RGB);
   cvtColor (Frames[idx2].Image, Im2, COLOR_GRAY2RGB);
   //char buff[256];
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

         {

            //Incorporamos a la distancia euclideana los centro de masa.
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
               if (tempDistMC < valor_min)
               {
                  //cout<<"frameIds "<<fIdx<<" distance: "<<distance<<" umbralHu: "<<umbralHu<<endl;
                  valor_min = tempDistMC;
                  index1 = i;
                  index2 = j;
                  // finalDCD = tempDistMC;
                  //finalHUD = distance;
               }
               if (distance < minHu)
                  minHu = distance;
            }
         }                      //if(distance < ...
      }                         //for (j=...
      if (valor_min >= 0)
      {
         /*
            circle(ImD1, Point(Frames[idx1].mc[index1].x,Frames[idx1].mc[index1].y), 10, Scalar(255, 0, 255),FILLED,LINE_8);
            circle(ImD2, Point(Frames[idx2].mc[index2].x,Frames[idx2].mc[index2].y), 10, Scalar(128, 255, 128),FILLED,LINE_8);
            snprintf(buff, 255, "dHu = %f, dEucSq = %f", sqrt(finalHUD), sqrt(finalDCD));
            putText(ImD1, String(buff), Point2i (50, ImD1.rows-50), FONT_HERSHEY_DUPLEX,1, Scalar(0,255,0),1,false);
            imshow("Figura1", ImD1);
            imshow("Figura2", ImD2);
            waitKey(0);
            if (waitKeyEx (30) > 0)
            break; 
          */
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


Mat fitLine (vector < Mat > pointsToFit, double tolEl)
{
   unsigned i;
   double epsilon = 0.1;
   double W = 1;
   double c = 0;
   double Af, Bf, Cf;
   double minEl;
   Mat M = Mat::zeros (3, 3, CV_64FC1);
   Mat N = M.clone ();
   Mat Vo =
      (Mat_ < double >(3, 3) << pow (epsilon, 2), 0, 0, 0, pow (epsilon, 2), 0,
       0, 0, 1);
   int conta = 0;

   minEl = 10e10;
   while (true)
   {
      for (i = 0; i < pointsToFit.size (); ++i)
      {
         Mat pts = pointsToFit[i].clone ();
         M += W * pts * pts.t ();
         N += W * Vo;
      }
      M = M / pointsToFit.size ();
      N = N / pointsToFit.size ();
#ifdef __VERBOSE__
      cout << "M = " << M-c*N << endl;
#endif      

      Mat eigenValues;
      Mat eigenVectors;

      eigen (M - c * N, eigenValues, eigenVectors);
#ifdef __VERBOSE__
      cout << "eigenValues  = " << eigenValues << endl ;
      cout << "eigenVectors = " << eigenVectors << endl << endl;
#endif      
      double el;
      unsigned int idx;
      Mat en;
      
      idx = 0; el = eigenValues.at < double >(0, 0);
      if (eigenValues.at < double >(1, 0) < el)
      {
         el = eigenValues.at < double >(1, 0);
         idx = 1;
      }
      if (eigenValues.at < double >(2, 0) < el)
      {
         el = eigenValues.at < double >(2, 0);
         idx = 2;
      }
      en = eigenVectors.row (idx).t ();

      if (abs(el) < minEl)
         minEl = abs(el);

      conta++;

      if (abs (el) < tolEl)
      {
         Af = en.at < double >(0, 0);
         Bf = en.at < double >(1, 0);
         Cf = en.at < double >(2, 0);
         break;
      }
      else
      {
         if (conta > 100)
         {
            cout << "Maximas iteraciones alcanzadas" << endl;
            break;
         }
      }
      cout <<  "Vo - " << Vo << endl << "en = " << en << endl; 
      W = 1.0 / en.dot (Vo * en);
      c = (c + el) / en.dot (N * en);
      // cout<<"conta = "<<conta<<"  c = "<<c<<" W = "<<W<<endl; 
   }
   Mat line = (Mat_ < double >(1, 3) << Af, Bf, Cf);
   return line;

}
