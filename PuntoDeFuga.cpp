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

String image_path;
String file;
RNG rng (12345);
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


vector < Point2f > findCorrespondences (Mat huMomentNew, Mat HuMomentsOld,
                                        vector < Point2f > mc_new,
                                        vector < Point2f > mc_old,
                                        int SizeNew, int SizeOld)
{
   const double umbralDistance = 0	;
   int index1 = 0;
   int index2 = 0;
   vector < Point2f > correspondences; //[SizeOld];
   double valor_min = 0;
   for (int i = 0; i < SizeOld; i++)
   {
      for (int j = 0; j < SizeNew; j++)
      {
         double distancePointsX = (mc_new[j].x - mc_old[i].x) * (mc_new[j].x - mc_old[i].x);
         double distancePointsY = (mc_new[j].y - mc_old[i].y) * (mc_new[j].y - mc_old[i].y);
         double distance = 0;
         for (int k = 0; k < 7; k++)
         {
            //Distancia Euclideana entre cada fila de momentos de HU.
            distance =
               distance + (huMomentNew.at < double >(j, k) - HuMomentsOld.at < double >(i, k)) *(huMomentNew.at < double >(j, k) - HuMomentsOld.at < double >(i, k));
         }

         //Incorporamos a la distancia euclideana los centro de masa.
         double tempDistMC = sqrt((mc_new[j].x - mc_old[i].x) * (mc_new[j].x - mc_old[i].x) + (mc_new[j].y - mc_old[i].y) * (mc_new[j].y - mc_old[i].y));
         distance = distance + tempDistMC ;
         //cout<<"tempDistMC: " <<tempDistMC << "del "<<i<<"i-ésimo objeto"<<endl;
         if (j == 0)
         {
            valor_min = distance;
            continue;
         }
         else
         {
            if (distance < valor_min && tempDistMC < umbralDistance)
            {
               //cout<<"distancePointsX:  "<<distancePointsX<<"     distancePointsY:  "<<distancePointsX<<endl;
               valor_min = distance;
               index1 = i;
               index2 = j;
               cout << "Distancia " << i << " -- " << j << "  = " << distance<< "distancePointsX+distancePointsY  "<<sqrt(distancePointsX+distancePointsY)
                  << endl;
            }
            else
            {
               continue;
            }
         }

      }
      correspondences.push_back (Point2f (index1, index2));
      index1 = 0;
      index2 = 0;
   }


   return correspondences;

}



int main (int argc, char **argv)
{
   int t = 0;
   int dilation_type = 2;
   int dilation_size = 2;
   bool Umbraliza = true;

   String dataFiles, OutDir;
   ifstream infile;
   Mat image;
   vector < vector < Point > >contours;
   vector < Vec4i > hierarchy;
/*FOURIER DESCRIPTORS VARIABLES*/

   unsigned int nDesc = 0;
   double reconError;
   vector < vector < Point2f >> frame_mc (100);
   vector < vector < Mat >> frame_HU (100);
   vector < Mat > tempMatHU (100);
   vector < int >contornoSize;


   ofstream fileOut("descriptorsFrame.res");

   if (argc < 3)
   {
      cerr << "Faltan argumentos:\n\n\tUso:\n\t\t " << argv[0] << " ListaArchivos DirectorioSalida [Umbraliza]"
           << endl << endl
           << "\tListaArchivos -> Archivo de texto que contiene lista de archivos a procesar"
           << endl;
   }
   dataFiles = String(argv[1]);
   OutDir = String(argv[2]) + "/";
   if (argc > 3)
      Umbraliza = false;

   infile.open(dataFiles);





   cout << "Finding descriptors... " << endl;
   Mat pointMat (1000, 1500, CV_8UC3, Scalar (0, 0, 0));

   namedWindow ("Output", 1);

   if (Umbraliza)
      namedWindow ("Umbralizada", 1);

   while (getline (infile, file))
   {
      istringstream iss (file);
      cout << file << endl;
      image_path = file;

      Mat image = imread (image_path, IMREAD_GRAYSCALE);
      if (!image.data)
      {
         cout << "Could not open or find the image" << std::endl;
         return -1;
      }


        Mat element = getStructuringElement (dilation_type,
         Size (2 * dilation_size + 1,
         2 * dilation_size + 1),
         Point (dilation_size,
         dilation_size)); 

      if (Umbraliza)
      {
         threshold (image, image,0, 255, THRESH_BINARY_INV | THRESH_OTSU);
         imshow ("Umbralizada", image);
      }

      if (!Umbraliza)
         dilate (image, image, element);

      findContours (image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); //Para HU Moments

      Mat imContours;
      /*Fourier Descriptors */

      {
         FourierDescriptor FD;

         Mat contoursFourier = image.clone ();
         FD.setContours (contoursFourier);
         FD.computeDescriptors ();
         nDesc = contours.size ();
         cout<<"nDesc: "<<nDesc<<endl;
         reconError = FD.reconstructContours (0.5);
         plotContours (contoursFourier, imContours, FD);
      }


      Mat drawing = Mat::zeros (image.size (), CV_8UC3);
      //drawing contours.
      for (unsigned int i = 0; i < contours.size (); i++)
      {
         Scalar color = Scalar (rng.uniform (0, 256), rng.uniform (0, 256),
                                rng.uniform (0, 256));
         drawContours (drawing, contours, (int) i, color, 2, LINE_8,
                       noArray ());
      }

      imwrite (OutDir + "contour" + file, drawing);
      //Variables para descriptores de momentos.
      //cout<<contours.size ()<<endl;
      vector < Moments > mu (contours.size ()); //Variable de interés.
      vector < Point2f > mc (contours.size ()); //Variable de interés.
      vector < double[7] > huMoments (contours.size ()); //Variable de interés
      vector < double >distances (contours.size ());
      Mat huMomentsMat (contours.size (), 7, CV_64FC1, Scalar (0));
      //Variables para descriptores de fourier.

      for (unsigned int i = 0; i < contours.size (); i++)
      {
         mu[i] = moments (contours[i]);

         mc[i] =
            Point2f (static_cast < float >(mu[i].m10 / (mu[i].m00 + 1e-5)),
                     static_cast < float >(mu[i].m01 / (mu[i].m00 + 1e-5)));
         HuMoments (mu[i], huMoments[i]);
         for (unsigned int j = 0; j < 7; ++j)
         {
            huMomentsMat.at < double >(i, j) =
               -1 * copysign (1.0,
                              huMoments[i][j]) *
               log10 (abs (huMoments[i][j]) + 1e-8);

         }
      }


      if (t == 0)
      {
         contornoSize.push_back (contours.size ());
         tempMatHU.push_back (huMomentsMat);
         frame_mc.push_back (mc);

         // cout<<"Aqui"<<endl;
         //fileOut<<contours.size ()<<"\t"<<huMomentsMat<<"\t"<<mc<<endl;
      }
      else
      {
         
         int contornoSizeRespawn = contornoSize.back ();
         //contornoSize.pop_back ();
         // cout<<"Size Contorno anterior:  "<<contornoSizeRespawn<<endl;
         //cout<<"Size Contorno actual: "<<contours.size();
         vector < Point2f > mcRespawn = frame_mc.back ();
         // cout<<"Centros de masa anteriores: "<<mcRespawn<<endl;
         //cout<<"Centros de masa actuales: "<<mc<<endl;
         //mcRespawn.pop_back ();

         Mat HuRespawn (contornoSizeRespawn, 7, CV_64FC1, Scalar (0));
         HuRespawn = tempMatHU.back ();
         //cout<<mcRespawn[0].x<<endl;
         //HuRespawn.pop_back ();
         cout << "countours.size() =    " << contours.size() << endl
              << "contornoSizeRespawn = " << contornoSizeRespawn << endl;
         vector < Point2f > correspondences =
            findCorrespondences (huMomentsMat, HuRespawn, mc, mcRespawn,
                                 contours.size (), contornoSizeRespawn);
         cout << correspondences << endl;
         pointMat = imContours.clone ();
         //Punto de Fuga 

         for (unsigned int i = 0; i < correspondences.size (); i++)
         {
            // cout<<"["<<Point2i(mcRespawn[correspondences[i].x])<<"::"<<mc[correspondences[i].y]<<"]"<<endl;
            Scalar color = Scalar (rng.uniform (0, 256), rng.uniform (0, 256),rng.uniform (0, 256));
            line (pointMat, Point2i (mcRespawn[correspondences[i].x]),Point2i (mc[correspondences[i].y]), color, 2, LINE_8);
            //cout<<"frame "<<t<<" --> "<<correspondences[i].x<<"------>"<<correspondences[i].y<<" color:  ("<<color<<")"<<endl;
           	//circle( pointMat,Point2i (mcRespawn[correspondences[i].x]),5,color,FILLED,LINE_8 );
           	//circle( pointMat,Point2i (mcRespawn[correspondences[i].y]),5,color,FILLED,LINE_8 );
         }




         // cout<<imContours.type()<<endl;



         imshow ("Output", pointMat);
         waitKey (0);
         if (waitKeyEx (30) > 0)
            break;

         //finaliza punto de fuga.

         contornoSize.push_back (contours.size ());
         frame_mc.push_back (mc);
         tempMatHU.push_back (huMomentsMat);
        // fileOut<<contours.size ()<<"\t"<<correspondences<<"\t"<<endl;
         //HuRespawn = tempMatHU.back();

         //cout<<huMomentsMat<<endl;
      }


      //Respaldo de los centros de masa de frames pasados.





      cout << "Secuencia: " << t << " con : " << contours.size () <<
         " objetos." << endl;
      t++;

   }

   cout << "Terminado" << endl;
   infile.close ();
   fileOut.close();
   return 0;
}
