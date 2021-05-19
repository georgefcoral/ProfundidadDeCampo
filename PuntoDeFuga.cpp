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


vector < Point3f > findCorrespondences (Mat huMomentsNew, Mat huMomentsOld,
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

Point3f getLineTwo(Point2i pt1, Point2i pt2){
	Point3f coeff;
	double m = (pt2.y - pt1.y)/(pt2.x-pt1.x);
	double b = (-m*pt1.x+pt1.y);
	double A,B,C;
	A = 1/b;
	B= m /b;
	C = 1;
	coeff = Point3f(A,B,C);
	return coeff;
}

Point2i escapePoint(vector<Point2i> pts1, vector<Point2i> pts2){
	Mat matrixA(pts1.size (),2,CV_64FC1,Scalar(0));
	Mat b (pts1.size (),1,CV_64FC1,Scalar(1));
	Mat output;

	for(unsigned int i = 0; i < pts1.size (); i++){

		matrixA.at<double>(i,0) =getLineTwo(pts1[i],pts2[i]).x;
		matrixA.at<double>(i,1) =getLineTwo(pts1[i],pts2[i]).y;

	}
	cout<<matrixA<<endl;
	cout<<b<<endl;
	solve(matrixA,b.t(),output,DECOMP_NORMAL);
	cout<<"Punto de fuga es:   "<<output<<endl;
	return Point2i(0,0);
}


int main (int argc, char **argv)
{
   int t = 0;
   int dilation_type = 2;
   int dilation_size = 2;
   bool Umbraliza = true;
   double umbralDistance = 50.;
  
   String dataFiles, OutDir;
   ifstream infile;
   Mat image;
   vector < vector < Point > >contours;
   //vector < Vec4i > hierarchy;
/*FOURIER DESCRIPTORS VARIABLES*/

   unsigned int nDesc = 0;
   double reconError;
   vector < vector < Point2f >> frame_mc (100);
   vector < vector < Mat >> frame_HU (100);
   vector < Mat > tempMatHU (100);
   vector < int >contornoSize;
   //vector <vector<double>>areasPrev(100);


   ofstream fileOut("descriptorsFrame.res");

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
      vector<float> labels;
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
         threshold (image, image,25, 255, THRESH_BINARY_INV );
         //erode (image, image, element);
       //  dilate (image, image, element);

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

      
      //Variables para descriptores de momentos.
      //cout<<contours.size ()<<endl;
      vector < Moments > mu (contours.size ()); //Variable de interés.
      vector < Point2f > mc_temp (contours.size ()); //Variable de interés.
      vector < Point2f > mc; //Variable de interés.
      vector < double[7] > momentsHu (contours.size ()); //Variable de interés
      vector < double >distances (contours.size ());
      Mat huMomentsMat (contours.size (), 7, CV_64FC1, Scalar (0));
      //Variables para descriptores de fourier.
      vector <double> areaObjects (contours.size ());
      int index =0;
      for (unsigned int i = 0; i < contours.size (); i++)
      {
         mu[i] = moments (contours[i]);

         mc_temp[i] =
            Point2f (static_cast < float >(mu[i].m10 / (mu[i].m00 + 1e-5)),
                     static_cast < float >(mu[i].m01 / (mu[i].m00 + 1e-5)));
         

          //areaObjects.push_back(contourArea(contours[i]));
         
  
         if(mu[i].m00>700 ){
         	HuMoments (mu[i], momentsHu[i]);

         	for (unsigned int j = 0; j < 7; ++j)
         		{
            huMomentsMat.at < double >(index, j) = -1 * copysign (1.0,momentsHu[i][j]) *log10 (abs (momentsHu[i][j]) + 1e-8);
         		}

         	areaObjects.push_back(mu[i].m00);
         	mc.push_back(mc_temp[i]);
         	labels.push_back(i);
         	//cout<<Point2i (mc_temp[i]);
         	//cout<<"   Objeto "<<i<<"  ---  "<<mu[i].m00<<endl;
         	//circle( imContours,Point2i (mc_temp[i]),2,Scalar(255,0,0),FILLED,LINE_8 );
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
         //cout << "countours.size() =    " << contours.size() << endl
           //   << "contornoSizeRespawn = " << contornoSizeRespawn << endl;
         vector < Point3f > correspondences =
            findCorrespondences (huMomentsMat, HuRespawn, mc, mcRespawn,labels, umbralDistance , t);
         cout << correspondences << endl;
         pointMat = imContours.clone ();
         //Punto de Fuga 
         vector <Point2i> pts1;
         vector <Point2i> pts2;
         for (unsigned int i = 0; i < correspondences.size (); i++)
         {
            // cout<<"["<<Point2i(mcRespawn[correspondences[i].x])<<"::"<<mc[correspondences[i].y]<<"]"<<endl;
            Scalar color = Scalar (rng.uniform (0, 256), rng.uniform (0, 256),rng.uniform (0, 256));
            pts1.push_back(Point2i (mcRespawn[correspondences[i].x]));
            pts2.push_back(Point2i (mc[correspondences[i].y]));
            line (imContours, Point2i (mcRespawn[correspondences[i].x]),Point2i (mc[correspondences[i].y]), color, 2, LINE_8);
            //cout<<"frame "<<t<<" --> "<<correspondences[i].x<<"------>"<<correspondences[i].y<<" color:  ("<<color<<")"<<endl;
           	//circle( pointMat,Point2i (mcRespawn[correspondences[i].x]),5,color,FILLED,LINE_8 );
           	//circle( pointMat,Point2i (mcRespawn[correspondences[i].y]),5,color,FILLED,LINE_8 );
         }

    	//Point2i fugaPoint = escapePoint(pts1,pts2);



         // cout<<imContours.type()<<endl;

         imshow ("Output", imContours);
         waitKey (0);
         if (waitKeyEx (30) > 0)
            break;

         //finaliza punto de fuga.

         contornoSize.push_back (mc.size ());
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
