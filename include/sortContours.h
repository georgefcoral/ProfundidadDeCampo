#ifndef __SORTCONTOURS__
#define __SORTCONTOURS__

#include "opencv2/opencv.hpp"
#include <vector>

void sortContours (std::vector<std::vector<cv::Point> > &contoursIn, std::vector<std::vector<cv::Point> > &contoursOut);

void filterSmallContours(std::vector < std::vector < cv::Point > >&contoursIn, double umbralArea);

void normHuMoments(double *Hu);

#endif
