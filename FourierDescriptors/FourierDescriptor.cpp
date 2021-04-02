#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <FourierDescriptor.h>
#include <math.h>
#include <string>


complexContour::complexContour()
{
   contour = contourF = tmp = 0;
   N = 0;
   form = formF = formT = RealImag;
   Center = cv::Point2f(0., 0.);
}

complexContour::complexContour(unsigned int n)
{
   form = formF = formT = RealImag;
   Center = cv::Point2f(0., 0.);
   contour = contourF = tmp = 0;
   N = 0;
   contour = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * 3 * n);
   if (contour)
   {
      contourF = contour + n;
      tmp = contourF + n;
      N = n;
   }
   else
      contour = contourF = tmp = 0;
}

//Copy constructor
complexContour::complexContour(const complexContour &C)
{
   N = 0;
   Copy(C);
}

complexContour &complexContour::operator = (const complexContour &C)
{
   Copy(C);

   return *this;
}

complexContour::~complexContour()
{
   if (contour)
      fftw_free((void *)contour);
}

void complexContour::Copy(const complexContour &C)
{
   if (N)
   {
      if (N == C.N)
      {
         memcpy (contour, C.contour, 3 * N * sizeof(fftw_complex));
         Center = C.Center;
         form = C.form;
         formF = C.formF;
         formT = C.formT;
         return;
      }
      else
         fftw_free((void *)contour);
   }

   Center = C.Center;
   form = C.form;
   formF = C.formF;
   formT = C.formT;
   N = C.N;

   if (N)
   {
      contour = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) *  3 * N);
      if (contour)
      {
         contourF = contour + N;
         tmp = contourF + N;
         memcpy (contour, C.contour, 3 * N * sizeof(fftw_complex));
      }
      else 
         contour = contourF = tmp = 0;
   }
   else
      contour = contourF = tmp = 0;
}


void complexContour::setLength(unsigned int n)
{
   if (N == n)
      return;
   if (N)
   {
      fftw_free((void *)contour);
      contour = contourF = tmp = 0;
   }
   N = 0;
   form = formF = formT = RealImag;
   Center = cv::Point2f(0., 0.);
   
   if (n)
   {
      contour = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) *  3 * n);
      if (contour)
      {
         contourF = contour + n;
         tmp = contourF + n;
         N = n;
      }
      else 
         contour = contourF = tmp = 0;
   }
   else
      contour = contourF = tmp = 0;
}

void complexContour::conv2MagPhase(contourSpace space)
{
  cmplxForm *frm;
  fftw_complex *cntr;
   
   if (!N)
      return;
   switch(space)
   {
      case Tmp:
         frm = &formT;
         cntr = tmp;
         break;
      case Fourier:
         frm = &formF;
         cntr = contourF;
         break;
      default:
         frm = &form;
         cntr = contour;
   }
   if (N && *frm == RealImag)
   {
      unsigned int i;
      double phase;

      for (i = 0; i < N; ++i)
      {
         phase = atan2(cntr[i][1], cntr[i][0]);
         cntr[i][0] = sqrt(pow(cntr[i][0],2)+pow(cntr[i][1],2));
         cntr[i][1] = phase;
      }
      *frm = MagPhase;
   }
}

void complexContour::conv2RealImag(contourSpace space)
{
   cmplxForm *frm;
   fftw_complex *cntr;
   
   if (!N)
      return;
   switch(space)
   {
      case Tmp:
         frm = &formT;
         cntr = tmp;
         break;
      case Fourier:
         frm = &formF;
         cntr = contourF;
         break;
      default:
         frm = &form;
         cntr = contour;
   }
   if (*frm == MagPhase)
   {
      unsigned int i;
      double mag, phase;

      for (i = 0; i < N; ++i)
      {
         mag = cntr[i][0];
         phase = cntr[i][1];
         cntr[i][0] = mag * cos(phase);
         cntr[i][1] = mag * sin(phase);
      }
      *frm = RealImag;
   }
}

void complexContour::printContour(const std::string &Nominal, contourSpace space)
{
   fftw_complex *cntr;
   unsigned int i;
   
   switch(space)
   {
      case Tmp:
         cntr = tmp;
         break;
      case Fourier:
         cntr = contourF;
         break;
      default:
         cntr = contour;
   }
   std::cout << Nominal << " ";
   for (i = 0; i < N; ++i)
      std::cout << "(" << cntr[i][0]<< "," << cntr[i][1] << ") ";
   std::cout << std::endl << std::endl;
}

//----------------------------------------

void FourierDescriptor::setContours(const cv::Mat &frame, cv::Point2f Center)
{
   unsigned int i;
   cv::findContours(frame, contourIn, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

   for (i = 0; i < contourIn.size();++i)
   {
      unsigned int contourLength;
      complexContour desc;

      Descriptor.clear();
      Descriptor.assign(contourIn.size(), desc);
      contourLength = contourIn[i].size();
      if (contourLength)
      {
         unsigned int j;
         std::vector <cv::Point>::iterator apu, end;

         desc.setLength(contourLength);
         apu = contourIn[i].begin();
         for (j = 0; j < contourLength; ++j)
         {
            desc.contour[j][0] = apu->x - Center.x; 
            desc.contour[j][1] = apu->y - Center.y; 
         }
         desc.Center = Center;
         Descriptor[i] = desc;
         contourDefined = true;
      }
   }
}

void FourierDescriptor::setContours(const cv::Mat &frame)
{
   unsigned int i;

   cv::findContours(frame, contourIn, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
   for (i = 0; i < contourIn.size();++i)
   {
      unsigned int contourLength;
      
      Descriptor.clear();
      contourLength = contourIn[i].size();
      if (contourLength)
      {
         double xm, ym;
         std::vector <cv::Point>::iterator apu, end;
         complexContour desc;

         desc.setLength(contourLength);
         xm = ym = 0.;
         apu = contourIn[i].begin();
         end = contourIn[i].end();
         for (; apu != end; ++apu)
         {
            xm += apu->x;
            ym += apu->y;
         }
         xm /= contourLength;
         ym /= contourLength;
         apu = contourIn[i].begin();
         for (unsigned int j = 0; j < contourLength; ++j, ++apu)
         {
            desc.contour[j][0] = apu->x - xm; 
            desc.contour[j][1] = apu->y - ym; 
         }
         desc.Center = cv::Point2f(xm, ym);
         Descriptor.push_back(desc);
         contourDefined = true;
      }
   }
}

void FourierDescriptor::computeDescriptor()
{
   unsigned int i;

   if (!contourDefined)
   {
      std::cerr << "Couldn't compute Descriptor without having seting a contour first" << std::endl; 
      return;
   }



   for (i = 0; i < contourIn.size();++i)
   {
      fftw_plan plan;

      plan = fftw_plan_dft_1d(Descriptor[i].N, Descriptor[i].contour, Descriptor[i].contourF, FFTW_FORWARD, FFTW_ESTIMATE);
      fftw_execute(plan);

      Descriptor[i].conv2MagPhase(Fourier);

   }
}

double FourierDescriptor::reconstructContour (unsigned int nDesc)
{
   unsigned int i, tope, cont;
   std::vector<complexContour>::iterator itD, endD;
   std::vector< std::vector<cv::Point> >::iterator itCO, itCI;
   double error;


   if (contourOut.size() != contourIn.size())
   {
      std::vector<cv::Point> vP;

      contourOut.clear();
      contourOut.assign(contourIn.size(),vP);
   }
   error = 0.l;
   cont = 0;
   itD = Descriptor.begin();
   endD = Descriptor.end();
   itCO = contourOut.begin();
   itCI = contourIn.begin();
   for (; itD != endD;++itD, ++itCO, ++itCI)
   {
      fftw_plan plan;

      if (itCO->size() != itD->N)
      {
         cv::Point P;

         itCO->clear();
         itCO->assign(itD->N, P);
      }

      itD->conv2MagPhase(Fourier);
      if (nDesc > itD->N)
         nDesc = itD->N;
      tope = itD->N - nDesc;
      memcpy (itD->tmp, itD->contourF, sizeof(fftw_complex) * itD->N);
      for (i = nDesc; i < tope; ++i)
         itD->tmp[i][0] = 0.;
      itD->formT = MagPhase;
      itD->conv2RealImag(Tmp);

      plan = fftw_plan_dft_1d(itD->N, itD->tmp, itD->contour, FFTW_BACKWARD, FFTW_ESTIMATE);
      fftw_execute(plan);

      for (i=0; i<itCO->size();++i)
      {
         (*itCO)[i] = cv::Point(int((itD->contour[i][0])/itD->N + itD->Center.x), int((itD->contour[i][1])/itD->N + itD->Center.y));
         error += pow((*itCO)[i].x-(*itCI)[i].x, 2.)+pow((*itCO)[i].y-(*itCI)[i].y, 2.);
      }
      cont += itCO->size();
   }
   return error / cont;
}

void FourierDescriptor::findContourLimits(int &xmin, int &xmax,int  &ymin, int &ymax)
{
   std::vector< std::vector<cv::Point> >::iterator itIn, itOut, endIn;

   if (!contourIn.size() || !contourOut.size())
   {
      std::cerr << "Error in FourierDescriptor::findContourLimits: empty contours" << std::endl;
      xmin = xmax = ymin = ymax = 0;
      return;
   }
   itIn = contourIn.begin();
   endIn = contourIn.end();
   itOut = contourOut.begin();

   xmin = xmax = (*itIn)[0].x;
   ymin = ymax = (*itIn)[0].y;
   for (;itIn != endIn; ++itIn, ++itOut)
   {
      std::vector<cv::Point>::iterator itRI, itRO, endRI;
      
      itRI = itIn->begin();
      itRO = itOut->begin();
      endRI = itIn->end();
      for (; itRI != endRI; itRI++, itRO++)
      {
         if (itRI->x < xmin)
            xmin = itRI->x;
         if (itRO->x < xmin)
            xmin = itRO->x;
         if (itRI->x > xmax)
            xmax = itRI->x;
         if (itRO->x > xmax)
            xmax = itRO->x;
         if (itRI->y < ymin)
            ymin = itRI->y;
         if (itRO->y < ymin)
            ymin = itRO->y;
         if (itRI->y > ymax)
            ymax = itRI->y;
         if (itRO->y > ymax)
            ymax = itRO->y;
      }
   }
}

void FourierDescriptor::translateContour(int dx, int dy, contourKind cK)
{
   std::vector< std::vector<cv::Point> >::iterator it, end;

   if ( (cK == In && !contourIn.size()) || (cK == Out && !contourOut.size()) )
   {
      std::cerr << "Error in FourierDescriptor::translateContour: empty contour" << std::endl;
      return;
   }
   if (cK == In)
   {
      it = contourIn.begin();
      end = contourIn.end();
   }
   else
   {
      it = contourOut.begin();
      end = contourOut.end();
   }

   for (;it != end; ++it)
   {
      std::vector<cv::Point>::iterator itR, endR;
      
      itR = it->begin();
      endR = it->end();
      for (; itR != endR; itR++)
      {
         itR->x += dx;
         itR->y += dy;
      }
   }
}


void plotContours(cv::Mat &image, cv::Mat &imContour, FourierDescriptor &FD, cv::Scalar colorIn, cv::Scalar colorOut, int hBorder, int vBorder)
{
   unsigned int i;
   int xmin, xmax, ymin, ymax, width, height;

   FD.findContourLimits(xmin, xmax, ymin, ymax);
   width = xmax - xmin;
   height = ymax - ymin;
   if (!height || !width)
   {
      std::cerr << "Error::plotContours: empty contour" << std::endl;
      return;
   }
   imContour = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
   for (i = 0; i < FD.contourIn.size(); ++i)
   {
      cv::drawContours (imContour, FD.contourIn, i, colorIn, 1);
      cv::drawContours (imContour, FD.contourOut, i, colorOut, 1);
   }
}
