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

using namespace cv;
using namespace std;

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;
const double PI = 3.141592653;
random_device rd;
default_random_engine eng(rd());
uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);

String image_path;
String file;
const String outDir = "Outputs/";


Mat cambia(Mat M)//Imagen Negativa.
{
    int i, j;
    double *ptr;

    for (i=0;i<M.rows;++i)
    {
        ptr = M.ptr<double>(i);
        for (j=0;j<M.cols;++j, ++ptr)
            *ptr = -*ptr+255;
    }
  	return M;
}

Mat gradienteMax(Mat image){
   Mat outImage(image.size(),CV_64FC1,Scalar(0));
   Mat w_1 = (Mat_<int>(3,3) << -1,0,1,-2,0,2,-1,0,1);
   Mat w_2 = (Mat_<int>(3,3) << -1,-2,-1,0,0,0,1,2,1);
   Mat w_3 = (Mat_<int>(3,3) << 0,1,2,-1,0,1,-2,-1,0);
   Mat w_4 = (Mat_<int>(3,3) << -2,-1,0,-1,0,1,0,1,2);

   Mat W1, W2, W3, W4;

   filter2D(image, W1, -1, w_1);
   filter2D(image, W2, -1, w_2);
   filter2D(image, W3, -1, w_3);
   filter2D(image, W4, -1, w_4);

   Mat output(image.rows,image.cols,CV_64FC1,Scalar(0));
   


   for(int i = 0; i < image.rows; i++){
      for(int j = 0; j<image.cols; j++){
         int temp = max( int(W1.at<uchar>(i,j)),int(W2.at<uchar>(i,j)));
         temp = max(temp,int(W3.at<uchar>(i,j)));
         temp = max(temp,int(W4.at<uchar>(i,j)));
         //cout<<int(w_1.at<uchar>(i,j))<<"--"<<int(w_2.at<uchar>(i,j))<<"--"<<int(w_3.at<uchar>(i,j))<<"--"<<int(w_4.at<uchar>(i,j))<<endl;
         output.at<double>(i,j) = temp;
         outImage.at<double>(i,j) = output.at<double>(i,j);
      }
   }
   //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
   //imwrite("Outputs/GMAX-"+file,outImage);
   return output;
}

double robertsGradient(Mat image,Mat &dst){
   double value = 0;
   Mat outImage(image.size(),CV_64FC1,Scalar(0));

   for(int i=1; i<image.rows-1; i++){
      for(int j = 1; j<image.cols-1; j++){
         value = value + pow(image.at<uchar>(i+1,j+1) - image.at<uchar>(i,j),2) +
                        pow(image.at<uchar>(i+1,j) - image.at<uchar>(i,j+1),2);
         outImage.at<double>(i,j) = pow(image.at<uchar>(i+1,j+1) - image.at<uchar>(i,j),2) +
                        pow(image.at<uchar>(i+1,j) - image.at<uchar>(i,j+1),2);
      }
   }
   dst = outImage.clone();
  // normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
   //imwrite("Outputs/RobertsG-"+file,outImage);
   return value;
}

double tenengrad(Mat image,Mat& dst){
   Mat outImage(image.size(),CV_64FC1,Scalar(0));
   Mat temp1, temp2;
   temp2 = image.clone();
   temp1 = image.clone();
   double value =0;
   Mat w_1 = (Mat_<int>(3,3) << -1,0,1,-2,0,2,-1,0,1);
   Mat w_2 = (Mat_<int>(3,3) << -1,-2,-1,0,0,0,1,2,1);
   filter2D(image, temp1, -1, w_1);
   filter2D(image, temp2, -1, w_2);

   for(int i=1; i<image.rows-1; i++){
      for(int j = 1; j<image.cols-1; j++){
      	outImage.at<double>(i,j) = pow(temp1.at<uchar>(i,j),2) + pow(temp2.at<uchar>(i,j),2);
         value = value + pow(temp1.at<uchar>(i,j),2) + pow(temp2.at<uchar>(i,j),2);
      }
   }
   //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
   dst = outImage.clone();

   return value;
}

double brennerGradient(Mat image,Mat& dst){
   double value = 0;
   Mat outImage(image.size(),CV_64FC1,Scalar(0));

   for(int i=1; i<image.rows-1; i++){
      for(int j = 1; j<image.cols-1; j++){
      	 outImage.at<double>(i,j) = pow(image.at<uchar>(i+2,j)-image.at<uchar>(i,j),2);
         value = value + pow(image.at<uchar>(i+2,j)-image.at<uchar>(i,j),2);

      }
   }
   //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
   dst = outImage.clone();

   return value;
}

double variance(Mat image,Mat& dst){
   double value =0;
   double mean = 0;
   Mat outImage(image.size(),CV_64FC1,Scalar(0));


   for(int i=0; i<image.rows; i++){
      for(int j = 0; j<image.cols; j++){
         mean = mean + image.at<uchar>(i,j);
      }
   }
   mean = mean/((image.cols)*(image.rows));

   for(int i=0; i<image.rows; i++){
      for(int j = 0; j<image.cols; j++){
		 outImage.at<double>(i,j) = pow(image.at<uchar>(i,j)-mean,2)/((image.cols)*(image.rows));
      	 //outImage.at<double>(i,j) = pow(image.at<uchar>(i,j),2) - pow(mean,2);
         value = value + pow(image.at<uchar>(i,j)-mean,2);
      }
   }
   
   //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
   //outImage = cambia(outImage);
   dst = outImage.clone();
   return value;
}



double TSobel(Mat image,Mat & dst , double alpha)
{
   
   Mat outImage;
   Mat g_image(image.rows,image.cols,CV_64FC1,Scalar(0));
   g_image = gradienteMax(image);
   Mat imageOtsu;
   int threshold_value = threshold(image, imageOtsu, 0, 255,THRESH_OTSU);

   double tw_value = 0;
   double media = 0;
   double varianza = 0;
   threshold_value = alpha*threshold_value;
   Mat Tw_image(image.rows-1,image.cols-1,CV_64FC1,Scalar(0));
   for(int i = 1; i < image.rows-1; i++){
      for(int j = 1; j<image.cols-1; j++){
         media = (image.at<uchar>(i-1,j-1) + image.at<uchar>(i,j-1) + image.at<uchar>(i+1,j-1) + 
            image.at<uchar>(i-1,j) + image.at<uchar>(i,j) + image.at<uchar>(i+1,j) + image.at<uchar>(i-1,j+1) + image.at<uchar>(i,j+1) + image.at<uchar>(i+1,j+1))/9.0;
         
         varianza = ((image.at<uchar>(i-1,j-1)-media)*(image.at<uchar>(i-1,j-1)-media) 
            + (image.at<uchar>(i,j-1)-media)*(image.at<uchar>(i,j-1)-media)
            + (image.at<uchar>(i+1,j-1)-media)*(image.at<uchar>(i+1,j-1)-media) 
            + (image.at<uchar>(i-1,j)-media)*(image.at<uchar>(i-1,j)-media) 
            + (image.at<uchar>(i,j)-media)*(image.at<uchar>(i,j)-media) 
            + (image.at<uchar>(i+1,j)-media)*(image.at<uchar>(i+1,j)-media) 
            + (image.at<uchar>(i-1,j+1)-media)*(image.at<uchar>(i-1,j+1)-media) 
            + (image.at<uchar>(i,j+1)-media)*(image.at<uchar>(i,j+1)-media) 
            + (image.at<uchar>(i+1,j+1)-media)*(image.at<uchar>(i+1,j+1)-media))/9.0;

         if( varianza >= threshold_value){
            Tw_image.at<double>(i,j) = g_image.at<double>(i,j);
         }else{
            Tw_image.at<double>(i,j) = 0;
         }

         tw_value = tw_value +(Tw_image.at<uchar>(i,j))*(Tw_image.at<uchar>(i,j));
      }
   }
   //threshold( Tw_image, Tw_image, 20, 255, 0);
   //normalize(Tw_image,outImage,0,255,NORM_MINMAX,-1,noArray());
   imwrite("Out.jpg",Tw_image);
   dst = Tw_image.clone();
   return tw_value;
}



double meanMat(Mat input){
	double sum=0;
	for(int i=0;i<input.rows;i++){
		for(int j=0;j<input.cols;j++){
			sum=sum+input.at<uchar>(i,j);
		}
	}
	return sum/(input.rows*input.cols);
}

double variance2(Mat input,Mat &dst)
{
	Mat outImage(input.size(),CV_64FC1,Scalar(0));
    double var=0;
    double meanInput = meanMat(input);
    double totalSize=input.rows*input.cols;
    for(int i=0;i<input.rows;i++){
    	for(int j=0;j<input.cols;j++){
    		int pixelVal=input.at<uchar>(i,j);
    		var=var+((pixelVal*pixelVal)-(meanInput*meanInput));
    		outImage.at<double>(i,j) = ((pixelVal*pixelVal)-(meanInput*meanInput))/(totalSize);
    	}
    }
    normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
    //cout<<outImage<<endl;
     dst = outImage.clone(); 
    return var;
}

double energyG(Mat input,Mat& dst){
	double var;
	Mat outImage(input.size(),CV_64FC1,Scalar(0));
    Mat dx = (Mat_ < double >(1, 2) << -1.0, 1.0);
    Mat dy = (Mat_ < double >(2, 1) << -1.0, 1.0);
    Mat gx(input.cols, input.rows, CV_64FC1, Scalar(0));
    Mat gy(input.cols, input.rows, CV_64FC1, Scalar(0));

    for(int i=0;i<input.rows-1;i++){
    	for(int j=0;j<input.cols-1;j++){
    		gx.at<double>(i,j)=input.at<uchar>(i+1,j)-input.at<uchar>(i,j);
    		gy.at<double>(i,j)=input.at<uchar>(i,j+1)-input.at<uchar>(i,j);
    		var = var + (gx.at<double>(i,j)*gx.at<double>(i,j)) +(gy.at<double>(i,j)*gy.at<double>(i,j));
    		outImage.at<double>(i,j) = (gx.at<double>(i,j)*gx.at<double>(i,j)) +(gy.at<double>(i,j)*gy.at<double>(i,j));
    	}
    }
    //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
    dst = outImage.clone(); 
 
    
	return var;
}



double energyL(Mat input, Mat& dst)
{
	double var = 0;
	Mat outImage(input.size(),CV_64FC1,Scalar(0));
    Mat kernel = (Mat_ < double >(3, 3) << -1.0, -4.0, -1.0,
		  -4.0, 20.0, -4.0,
		  -1.0, -4.0, -1.0);
    Mat laplacianEImage;
    filter2D(input, laplacianEImage, CV_64FC1, kernel);
    for(int i=0;i<input.rows-1;i++){
    	for(int j=0;j<input.cols-1;j++){
    		var = var + laplacianEImage.at<double>(i,j)*laplacianEImage.at<double>(i,j);
    		outImage.at<double>(i,j) = laplacianEImage.at<double>(i,j)*laplacianEImage.at<double>(i,j);
    	}
    }
    //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
    dst = outImage.clone();
    return var;

}


int main(int argc, char **argv)
{
   if (argc < 2)
   {
       cerr << "Faltan Parámetros:\n\n\tUso: ./ejecutableFocusingMeasures path_images \n\n";
       cerr << endl; 
      exit (1);
    }
   String image_path = String (argv[1]);
   Mat img = imread(image_path, IMREAD_GRAYSCALE);
   if(img.empty())
   {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
   }

   Mat dst;
   double temp = TSobel(img,dst,1);
   cout<<dst.type()<<endl;
   namedWindow("Frame", WINDOW_NORMAL);
   imshow("Frame",dst);
   waitKey(0);
   destroyWindow ("Frame");

return 0;
}

