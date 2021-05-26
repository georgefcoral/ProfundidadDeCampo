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
#include<fftw3.h>
#include <FourierDescriptor.h>



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


vector < Point3f > findCorrespondences (const Mat &huMomentsNew,const Mat &huMomentsOld,
                                        vector < Point2f > mc_new,
                                        vector < Point2f > mc_old,
                                        vector<float> labels,
                                        double umbralDistance = 50., int fIdx=0)
{
   int SizeNew = mc_new.size();
   int SizeOld = mc_old.size();
   int index1 = 0;
   int index2 = 0;
   vector < Point3f > correspondences; //[SizeOld];
   double valor_min = 0;
   double lambda = 5;
   ofstream fDist;
  
   
   if (fIdx == 1)
      fDist.open("Distancias.txt");
   else
      fDist.open("Distancias.txt", std::ios_base::app);

   fDist << "\% frameIdx sizeOld, sizeNew, idxOld, idxNew, MomemtsOld MomentsNew, DistMoments_ij DistCentro_ij" << endl;
   for (int i = 0; i < SizeOld; i++)
   {
      valor_min = -1.;
      for (int j = 0; j < SizeNew; j++)
      {
         double distance = 0;

         fDist << fIdx << ", " << SizeOld << "," << SizeNew << ", "
               << i << "," << j << ", ";

         for (int k = 0; k < 7; k++)
            fDist << huMomentsOld.at < double >(i, k) << ",";
         fDist << " ";
         for (int k = 0; k < 7; k++)
            fDist << huMomentsNew.at < double >(j, k) << ",";
         fDist << " ";
         for (int k = 0; k < 7; k++)
            //Distancia Euclideana entre cada fila de momentos de HU.
            distance +=
                (huMomentsNew.at < double >(j, k) - huMomentsOld.at < double >(i, k)) *(huMomentsNew.at < double >(j, k) - huMomentsOld.at < double >(i, k));
         

         fDist << mc_old[i].x << "," << mc_old[i].y << ", " << mc_new[j].x
               << "," << mc_new[j].y << ", " << distance << ",";

         //Incorporamos a la distancia euclideana los centro de masa.
         double tempDistMC = (mc_new[j].x - mc_old[i].x) * (mc_new[j].x - mc_old[i].x) + (mc_new[j].y - mc_old[i].y) * (mc_new[j].y - mc_old[i].y);
         distance = sqrt(distance + lambda * tempDistMC );

         fDist << tempDistMC << ", " << distance  << ","<<labels[i]<<endl;
         //cout<<"Area del objeto: "<<areaObjects[i]<<endl;
         //cout<<i<<" - "<<j<<"  tempDistMC   ::    "<<sqrt(tempDistMC)<<endl;
         if( sqrt(tempDistMC) < umbralDistance)
         {
         
            if (j == 0)
            {
               valor_min = distance;
               continue;
            }
            else
               if (distance < valor_min)
               {
                  valor_min = distance;
                  index1 = i;
                  index2 = j;
               }
         }
      }
      if (valor_min >= 0)
      {
         //cout << "valor_min = " << valor_min << endl;
         correspondences.push_back (Point3f (index1, index2,labels[index1]));
         index1 = index2 = 0;
      }
   }
   fDist.close();
   return correspondences;

}