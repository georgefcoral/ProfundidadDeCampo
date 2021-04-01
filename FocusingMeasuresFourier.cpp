#include<opencv2/core/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>
#include "opencv2/opencv.hpp"
#include<iostream>
#include<fftw3.h>
#include<cstring>
#include<complex>
#include<vector>
#include<cmath>
#include <fstream>

const double PI = 3.141592653;


using namespace std;
using namespace cv;
String lectura;

double meanMat(Mat input){
	double sum=0;
	for(int i=0;i<input.rows;i++){
		for(int j=0;j<input.cols;j++){
			sum=sum+input.at<uchar>(i,j);
		}
	}
	return sum/(input.rows*input.cols);
}

double variance(Mat input)
{
	Mat outImage(input.size(),CV_64FC1,Scalar(0));
    double var=0;
    double meanInput = meanMat(input);
    double totalSize=input.rows*input.cols;
    for(int i=0;i<input.rows;i++){
    	for(int j=0;j<input.cols;j++){
    		int pixelVal=input.at<uchar>(i,j);
    		var=var+((pixelVal*pixelVal)-(meanInput*meanInput));
    		outImage.at<double>(i,j) = ((pixelVal*pixelVal)-(meanInput*meanInput));
    	}
    }
    //normalize(outImage,outImage,0,255,NORM_MINMAX,-1,noArray());
    imwrite("Outputs/Subbarao_variance"+lectura,outImage);
    //cout<<outImage<<endl;

    cout<<"Varianza: "<<var<<endl;
    return var;
}


double energyG(Mat input){
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
    imwrite("Outputs/energyG-"+lectura,outImage);
    cout<<"EnergyG = "<<var<<endl;
	return var;
}



double energyL(Mat input)
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
    imwrite("Outputs/energyL-"+lectura,outImage);
    cout<<"EnergyL = "<<var<<endl;
    return var;

}

//Filtro pasa-bajas.
Mat lowPass(Mat spaceImage, int umbral)
{
    int m = spaceImage.rows;
    int n = spaceImage.cols;
    Mat filterMat(m, n, CV_64FC1, Scalar(0));

	for (int i = 0; i < m / 2; i++) {
		for (int j = 0; j < n / 2; j++) {
	    	double d = i * i + j * j;
	    	if (d < umbral * umbral) {
				filterMat.at < double >(i, j) = 1;
				filterMat.at < double >(i, n - 1 - j) = 1;
				filterMat.at < double >(m - 1 - i, j) = 1;
				filterMat.at < double >(m - 1 - i, n - 1 - j) = 1;
	    	}	
		}
    }

    filterMat = filterMat.mul(spaceImage);
    return filterMat;
}

double gauss(double x, const double media, const double sigma ){
	double f = (1.0/sigma*sqrt(2)*(3.1416))*exp(-((x-media)*(x-media))/(2.0*sigma*sigma));
	return f;
}


//Mat Tw_image (image.rows - 1, image.cols - 1, CV_8UC1, Scalar (0));
int main(int argc, char **argv)
{
    Mat input, output;

    /*if (argc != 2) {
	cout << "Run ./test myImage.format umbral_value" << endl;
	exit(1);
    }*/
//input = imread(argv[1],IMREAD_GRAYSCALE);
    ofstream measure("measures_fourier.res");
    ifstream myFile("myFile.txt");
    
    int count = 0;

   
    
    while (getline(myFile, lectura)) {
	String image_path = string(argv[1]) + "/"+lectura;
    //String image_path = "fondoEnfoco.jpg";
    cout<<image_path<<endl;
	input = imread(image_path, IMREAD_GRAYSCALE);
    
   // cout<<"Matri con distribución gaussiana"<<endl<<matrizGaussiana<<endl;
	copyMakeBorder(input, input, int ((input.cols - input.rows) / 2),int ((input.cols - input.rows) / 2), 0, 0, BORDER_CONSTANT, 0);
	//int umbral = stoi(argv[1]);
	cout<<input.rows<<endl;
	Mat gaussianKernel = getGaussianKernel(input.rows,0.1,CV_64F);
    Mat matrizGaussiana = gaussianKernel*gaussianKernel.t();
	//Para imagen de entrada.
	fftw_complex *complex_in, *complex_out;
	fftw_plan p,p2;
	//Para imagen Gaussiana.
	fftw_complex *complex_in2, *complex_out2;
	//fftw_plan p;
	complex_in =
	    (fftw_complex *) fftw_malloc(input.rows * input.cols *
					 sizeof(fftw_complex));
	complex_out =
	    (fftw_complex *) fftw_malloc(input.rows * input.cols *
					 sizeof(fftw_complex));
	complex_in2 =
	    (fftw_complex *) fftw_malloc(matrizGaussiana.rows * matrizGaussiana.cols *
					 sizeof(fftw_complex));
	complex_out2 =
	    (fftw_complex *) fftw_malloc(matrizGaussiana.rows * matrizGaussiana.cols *
					 sizeof(fftw_complex));
	int N = input.rows;
	int M = input.cols;
//[0] es la parte real.
//[1] es la parte imaginaria.
	for (int i = 0; i < N; i++) {
	    for (int j = 0; j < M; j++) {
		complex_in[i * M + j][0] = input.at < uchar > (i, j);
		complex_in2[i * M + j][0] = matrizGaussiana.at < double > (i, j);
	    }
	}

	p = fftw_plan_dft_2d(N, M, complex_in, complex_out, FFTW_FORWARD,FFTW_ESTIMATE);
	fftw_execute(p);

	p2 = fftw_plan_dft_2d(N, M, complex_in2, complex_out2, FFTW_FORWARD,FFTW_ESTIMATE);
	fftw_execute(p2);
/*for (int i = 0; i < N; i++) {
	for (int j = 0; j < M; j++) {

		cout<<complex_out2[i*N+j][0]<<"   +  "<<complex_out2[i*N+j][1]<<" j\t"<<endl;
	    }
	    //cout<<endl;
	}

for (int i = 0; i < N; i++) {
	for (int j = 0; j < M; j++) {

		cout<<complex_out[i*N+j][0]<<"   +  "<<complex_out[i*N+j][1]<<" j\t"<<endl;
	    }
	    //cout<<endl;
	}*/
//*--------------------------Preparado para el filtrado------------------------------*/
Mat magnitute_1(N,M,CV_64FC1,Scalar(0));
Mat phase_1(N,M,CV_64FC1,Scalar(0));

Mat magnitute_2(N,M,CV_64FC1,Scalar(0));
Mat phase_2(N,M,CV_64FC1,Scalar(0));

for(int i=0; i<N; i++){
	for(int j = 0; j<M; j++){
		magnitute_1.at<double>(i,j) = sqrt((complex_out[i*M+j][0]*complex_out[i*M+j][0]) + (complex_out[i*M+j][1]*complex_out[i*M+j][1]));
		phase_1.at<double>(i,j) = atan2(complex_out[i*M+j][1] , complex_out[i*M+j][0]);
		magnitute_2.at<double>(i,j) = sqrt((complex_out2[i*M+j][0]*complex_out2[i*M+j][0]) + (complex_out2[i*M+j][1]*complex_out2[i*M+j][1]));
		phase_2.at<double>(i,j) = atan2(complex_out2[i*M+j][1] , complex_out2[i*M+j][0]);
	}
	//cout<<endl;
}


 Mat magnitudFiltered(N,M,CV_64FC1,Scalar(0));
 Mat phaseFiltered(N,M,CV_64FC1,Scalar(0));
 magnitudFiltered = magnitute_1.mul(magnitute_2);
 //phaseFiltered = phase_1.mul(phase_2);
 phaseFiltered = phase_1 + phase_2;

for(int i=0; i<N; i++){
	for(int j = 0; j<M; j++){
		complex_out[i*M+j][0] = (magnitudFiltered.at<double>(i,j)*cos(phaseFiltered.at<double>(i,j)));
		complex_out[i*M+j][1] = (magnitudFiltered.at<double>(i,j)*sin(phaseFiltered.at<double>(i,j)));
	}
}

//*--------------------------Finaliza filtrado------------------------------*/

	p = fftw_plan_dft_2d(M, N, complex_out, complex_in, FFTW_BACKWARD,
			     FFTW_ESTIMATE);

	fftw_execute(p);

	Mat DFTI(N, M, CV_64FC1, Scalar(0));

	for (int i = 0; i < N; i++) {
	    for (int j = 0; j < M; j++) {
		DFTI.at < double >(i, j) =
		    sqrt(complex_in[i * M + j][0] * complex_in[i * M + j][0] +  complex_in[i * M + j][1] * complex_in[i * M + j][1]);
	    }
	}
	DFTI = sqrt(2*(PI))*DFTI;
	DFTI  = DFTI (Rect(0, 0, DFTI.cols & -2, DFTI.rows & -2));
	int cx = DFTI.rows/2;
	int cy = DFTI.cols/2;
	Mat q0 (DFTI, Rect(0,0,cx,cy));
	Mat q1 (DFTI ,Rect(cx,0,cx,cy));
	Mat q2 (DFTI ,Rect(0,cy,cx,cy));
	Mat q3 (DFTI ,Rect(cx,cy,cx,cy));

	Mat temp;
	q0.copyTo(temp);
	q3.copyTo(q0);
	temp.copyTo(q3);

	q1.copyTo(temp);
	q2.copyTo(q1);
	temp.copyTo(q2);

	normalize(DFTI, DFTI, 0, 255, NORM_MINMAX, -1);
   //cout<<DFTI<<endl;
	fftw_destroy_plan(p);
	//fftw_destroy_plan(p2);
	fftw_free(complex_in);
	fftw_free(complex_out);
	fftw_free(complex_in2);
	fftw_free(complex_out2);
	Mat gray(N, M, CV_8UC1, Scalar(0));
	DFTI.convertTo(gray, CV_8UC1);
//gray es imagen filtrada.
//Aplicar medidas de enfoque a gray
	double M4 = variance(DFTI);
	double M5 = energyG(DFTI);
	double M6 = energyL(DFTI);
	//measure << M1 << "\t" << M2 << "\t" << M3 << "\t" << M4 << "\t" << M5 << "\t" << M6 <<endl;

	count++;
	cout << "Frame: "<<count << endl;
	
	
	
	namedWindow("Imagen original", 1);
	imshow("Imagen original", input);
	namedWindow("Output", 1);
	imshow("Output", gray);
	waitKey(500);
	if (waitKeyEx(30) > 0)
        break;
    


   }
    measure.close();
    myFile.close();
    return 0;
}
