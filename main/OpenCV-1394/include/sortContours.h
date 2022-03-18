#ifndef __SORTCONTOURS__
#define __SORTCONTOURS__

#include "opencv2/opencv.hpp"
#include <vector>
/*!
\fn sortContours()
\brief Función que realiza el ordenamiento de los contornos de acuerdo a sus centros de masa y sus momentos.
\param vector<vector<Point>>> &contoursIn
\param vector<vector<Point>>> &contoursOut
*/
void sortContours (std::vector<std::vector<cv::Point> > &contoursIn, std::vector<std::vector<cv::Point> > &contoursOut);
/*!
\fn filterSmallContours
\brief Remueve contornos inválidos, objetos de tipo outliers.
\param vector<vector<Point>>> &contoursIn
\param double umbralArea
*/
void filterSmallContours(std::vector < std::vector < cv::Point > >&contoursIn, double umbralArea);
/*!
\fn normHuMoments(double *Hu)
\brief Normaliza los momentos de HU.
\param double *Hu
*/
void normHuMoments(double *Hu);

#endif
