#ifndef __deepFunctions__
#define __deepFunctions__
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
Mat cambia (Mat M);

vector < Point3f > findCorrespondences2 (vector<frameData> Frames, int idx1, int idx2, double umbralHu = 500, double umbralDistance = 60., int fIdx=0);
Point3f getLine(vector<Point2f> points);
Mat getCoeffLine(Point3f params);

Mat fitLine(vector<Mat> pointsToFit);
#endif
