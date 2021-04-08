#ifndef __FourierDescriptor__
#define __FourierDescriptor__

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <fftw3.h>
#include <vector>
#include <string>

enum cmplxForm {RealImag=0, MagPhase};
enum contourSpace {Signal, Fourier, Tmp};
enum contourKind {In, Out};

struct complexContour
{
   fftw_complex *contour;   //Non-Fourier Space
   fftw_complex *contourF;  //Fourier Space
   fftw_complex *tmp;  //Temporary contour.
   cv::Point2f Center;
   unsigned int N;
   cmplxForm form, formF, formT;

   
   complexContour();
   complexContour(unsigned int n);
   complexContour(const complexContour &C);
   complexContour &operator = (const complexContour &C);
   ~complexContour();
   void Copy(const complexContour &C);

   void setLength(unsigned int n);
   void conv2MagPhase(contourSpace space);
   void conv2RealImag(contourSpace space);
   void printContour(const std::string &Nominal, contourSpace space);
};

struct FourierDescriptor
{
   std::vector < std::vector<cv::Point> > contourIn, contourOut;
   std::vector < complexContour> Descriptor;
   bool contourDefined;
   inline FourierDescriptor() {contourDefined = false;}

   void setContours(const cv::Mat &frame, cv::Point2f Center);
   void setContours(const cv::Mat &frame);
   void computeDescriptors();
   double reconstructContours (double pDesc);
   void findContourLimits(int &xmin, int &xmax,int  &ymin, int &ymax);
   void translateContour(int dx, int dy, contourKind cK);

};

void plotContours(cv::Mat &image, cv::Mat &imContour, FourierDescriptor &FD, cv::Scalar colorIn = cv::Scalar(255, 127,64), cv::Scalar colorOut = cv::Scalar(64, 254,127), int hBorder = 10, int vBorder = 10);


#endif
