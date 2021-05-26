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
#include "deepFunctions.cpp"

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
   double umbralDistance = 50.;
  
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
         cerr << "El umbral minimo de distancia tiene que ser mayor que 0"
              << endl;
         exit(1);
      }
      if (argc > 4)
         Umbraliza = false;
   }


   infile.open(dataFiles);
   
   
   namedWindow ("Output", 1);
   if (Umbraliza)
      namedWindow ("Umbralizada", 1);

   while (getline (infile, file))
   {
      vector<float> labels;
      istringstream iss (file);
      cout <<"file:\t"<<file<<endl;
      image_path = file;
      Mat image = imread (image_path, IMREAD_GRAYSCALE);
      rowsGlobal = image.rows;
      colsGlobal = image.cols;
      if (!image.data)
      {
         cout << "Could not open or find the image" << std::endl;
         return -1;
      }

      	//Elemento necesario para el ajuste de dilatación.
         Mat element = getStructuringElement (dilation_type,
         Size (2 * dilation_size + 1,
         2 * dilation_size + 1),
         Point (dilation_size,
         dilation_size)); 

      if (Umbraliza)
      {
         threshold (image, image,25, 255, THRESH_BINARY_INV );
         imshow ("Umbralizada", image);
      }

      if (!Umbraliza)
         dilate (image, image, element);

      findContours (image, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); //Para HU Moments

      Mat imContours;//Variable para mostrar los resultados en imshow().
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

      vector < Moments > mu (contours.size ()); //Variable de interés.
      vector < Point2f > mc_temp (contours.size ()); //Variable de interés.
      vector < Point2f > mc; //Variable de interés.
      vector < double[7] > momentsHu (contours.size ()); //Variable de interés
      vector < double >distances (contours.size ());
      Mat huMomentsMat (contours.size (), 7, CV_64FC1, Scalar (0));
      //Variables para descriptores de fourier.
      vector <double> areaObjects (contours.size ());
      int index =0;

      //Hallamos los contornos por cada uno de los objetos,
      //Se verifica con respecto al área si es un objeto válido.
      for (unsigned int i = 0; i < contours.size (); i++)
      {
         mu[i] = moments (contours[i]);

         mc_temp[i] =
            Point2f (static_cast < float >(mu[i].m10 / (mu[i].m00 + 1e-5)),
                     static_cast < float >(mu[i].m01 / (mu[i].m00 + 1e-5)));        
         if(mu[i].m00>umbralArea ){//Verificación.
         	HuMoments (mu[i], momentsHu[i]);

         	for (unsigned int j = 0; j < 7; ++j)
         		{
            huMomentsMat.at < double >(index, j) = -1 * copysign (1.0,momentsHu[i][j]) *log10 (abs (momentsHu[i][j]) + 1e-8);
         		}

         	areaObjects.push_back(mu[i].m00);
         	mc.push_back(mc_temp[i]);
         	labels.push_back(i);
         	cv::putText(imContours,to_string(i),Point2i (mc_temp[i]),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),1,false);
         	index++;
         }
      }

      imwrite (OutDir + "contour" + file, drawing);
      if (t == 0)
      {
         contornoSize.push_back (mc.size ());
         tempMatHU.push_back (huMomentsMat);
         frame_mc.push_back (mc);
      }
      else
      {
         int contornoSizeRespawn = contornoSize.back ();
         vector < Point2f > mcRespawn = frame_mc.back ();
         Mat HuRespawn (contornoSizeRespawn, 7, CV_64FC1, Scalar (0));
         HuRespawn = tempMatHU.back ();
         vector < Point3f > correspondences = findCorrespondences (huMomentsMat,
         															HuRespawn,
         															mc,
         															mcRespawn,
         															labels,
         															umbralDistance ,
         															t);

         pointMat = imContours.clone ();
         if(maxCorrespondences<correspondences.size ()){
         	maxCorrespondences =correspondences.size ();
         }
         for (unsigned int i = 0; i < correspondences.size (); i++)
         {
            // cout<<"["<<Point2i(mcRespawn[correspondences[i].x])<<"::"<<mc[correspondences[i].y]<<"]"<<endl;

            Scalar color = Scalar (rng.uniform (0, 256), rng.uniform (0, 256),rng.uniform (0, 256));
            globalCorr[i].push_back(correspondences[i].y);
            mcGlobal[i].push_back(Point2i (mc[correspondences[i].y]));
            line (imContours, Point2i (mcRespawn[correspondences[i].x]),Point2i (mc[correspondences[i].y]), color, 2, LINE_8);
         }

        imshow ("Output", imContours);
        waitKey (0);
        if (waitKeyEx (30) > 0)
            break;
         contornoSize.push_back (mc.size ());
         frame_mc.push_back (mc);
         tempMatHU.push_back (huMomentsMat);
      }
     cout << "Secuencia: " << t << " con : " << contours.size () <<" objetos." << endl;
      t++;
   }
				/********************INICIA AJUSTE DE LINEA********************/
   Mat line;
   vector <Mat> linesH;
   vector <Point2i> ptsLine(mcGlobal.size());
   Mat previa (rowsGlobal, colsGlobal,CV_8UC1,Scalar(0));
   Mat A(18,2,CV_32FC1,Scalar(0));
   Mat b(18,1,CV_32FC1,Scalar(0));
    for(unsigned int i=0; i<maxCorrespondences;i++){
    		cout<<"Points: "<<mcGlobal[i].t()<<endl;
   			fitLine(mcGlobal[i],line,DIST_L2,0,0.01,0.01);
   			cout<<"Output fitLine: "<<line.t()<<endl;

   			double k = line.at<float>(1,0)/line.at<float>(0,0);//Pendiente de la recta.
   			double point1 = line.at<float>(2,0);//Abscisa de la recta.
   			double point2 = line.at<float>(3,0);//Ordenada de la recta.
   			//Dada la ecuación de la recta y - y0 = k ( x - x0)
   			//A = -1, B = k^-1 , C = x0 - y0/k 
   			Mat homogenLine = (Mat_<float>(4, 1) << -1,-1/k,point1-(point2/k),1.0);
   			A.at<float>(i,0) =-1;
   			A.at<float>(i,1) =-1/k;
   			b.at<float>(i,0) = point1-(point2/k);

   			linesH.push_back(homogenLine);
   		//}
   		//cout<<endl;
   }
   Mat M(4,4,CV_32FC1,Scalar(0));
//   cout<<A<<endl;
//   cout<<b<<endl;
   //Mat x;
   //solve(A,b,x,DECOMP_EIG);
   //circle( previa,Point(x),7,Scalar(255,0,0),FILLED,LINE_8 );
   //cout<<"x: "<<x<<endl;
  /* namedWindow("3",1);
   imshow("3",previa);
   waitKey(0);*/
   for(unsigned int i=0; i<linesH.size(); i++){
   		M = M + linesH[i]*linesH[i].t();
   }
   
   M = M / maxCorrespondences;
   Mat eigenValues;
   Mat eigenVectors;
   eigenNonSymmetric(M,eigenValues,eigenVectors);
  // cout<<eigenVectors<<endl;
  // cout<<eigenValues<<endl;
   infile.close ();
   fileOut.close();
   return 0;
}




