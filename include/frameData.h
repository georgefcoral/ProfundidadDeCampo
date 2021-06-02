#ifndef __FRAMEDATA__
#define __FRAMEDATA__

#include "opencv2/opencv.hpp"
#include <vector>
#include <cstring>
#include <string>
#include <momHu.h>

using namespace std;
using namespace cv;

struct frameData
{
   string fileName;
   Mat Image;
   Mat thImage;
   vector < vector < Point > >contours;
   vector < Moments > mu; //momentos.
   vector <momHu> momentsHu;//momentos de Hu.
   vector < Point2f > mc; //Centros de Masa.
   vector <float> areas;
   frameData()
   {
   }
   frameData(const frameData &F)
   {
      fileName = F.fileName;
      Image = F.Image.clone();
      thImage = F.thImage.clone();
      contours = F.contours;
      mu = F.mu;
      momentsHu  = F.momentsHu;
      mc = F.mc;
      areas = F.areas;
   }
   void operator = (const frameData &F)
   {
      fileName = F.fileName;
      Image = F.Image.clone();
      thImage = F.thImage.clone();
      contours = F.contours;
      mu = F.mu;
      momentsHu  = F.momentsHu;
      mc = F.mc;
      areas = F.areas;
   }
};

#endif
