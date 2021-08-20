#ifndef __deepFunctions2__
#define __deepFunctions2__
#include "opencv2/opencv.hpp"
#include <vector>

using namespace std;
using namespace cv;
/*!
\fn Mat cambia (Mat M) 
\brief Retorna el negativo de una imagen.
\param Mat m
*/
Mat cambia (Mat M);

/*!
\fn findCorrespondences2()
\brief Retorna un vector de tipo Point3f con las correspondencias entre cada par de imágenes.
\param vector < frameData > Frames
\param int idx1
\param int idx2
\param double umbralHu
\param double umbralDistance
\param int fIdx
*/

vector < Point3f > findCorrespondences2 (vector<frameData> Frames, int idx1, int idx2, double umbralHu = 500, double umbralDistance = 60., int fIdx=0);

/*!
\fn fitLine (vector < Mat > pointsToFit, double tolEl)
\brief Realiza el ajuste de líneas\puntos de acuerdo al algoritmo propuesto por Kanatani.
\param vector < Mat > pointsToFit
\param double tolEl
*/

Mat fitLine(vector<Mat> pointsToFit, double tolEl = 0.000003 );
#endif
