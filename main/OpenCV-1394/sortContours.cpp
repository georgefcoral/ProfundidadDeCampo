#include "opencv2/opencv.hpp"
#include <cstdlib>

using namespace std;
using namespace cv;

struct centros
{
   cv::Point2f P;
   unsigned int idx, height;
};

// Esta función de comparación asume que los objetos a ordenarse
// se encuentran alineados, y que el alineamiento es horizontal.
int cmpCentros (const void *a, const void *b)
{
   centros *A, *B;
   double meanHeight;

   A = (centros *) a;
   B = (centros *) b;

   meanHeight = 0.5 * (A->height+B->height);

   if (fabs(A->P.y - B->P.y) < meanHeight) //Si estan alineados.
   {
      if (A->P.x < B->P.x)
         return -1;
      else
      {
         if (A->P.x > B->P.x)
            return 1;
         else
            return 0;
      }
   }
   else
   {
      if (A->P.y < B->P.y)
         return -1;
      else
      {
         if (A->P.y > B->P.y)
            return 1;
         else
         {
            if (A->P.x < B->P.x)
               return -1;
            else if (A->P.x > B->P.x)
               return 1;
         }
      }
   }
   return 0;
}

void filterSmallContours(vector < vector < Point > >&contoursIn, double umbralArea)
{
   Moments mo;
   unsigned int nContours;
   unsigned int i;
   vector < vector < Point > > contoursOut;

   nContours = contoursIn.size ();

   if (!nContours)
      return;

   for (i = 0; i < nContours; ++i)
   {
      
      mo = moments (contoursIn[i]);
      if (mo.m00 >= umbralArea)
         contoursOut.push_back (contoursIn[i]);
   }
   contoursIn.clear();
   contoursIn = contoursOut;
}

void sortContours (vector < vector < Point > >&contoursIn,
                   vector < vector < Point > >&contoursOut)
{
   Moments mo;
   centros *C;
   cv::Point2f cM;
   unsigned int i, j, idx;
   unsigned int nContours;
   int val, max, min;

   nContours = contoursIn.size ();

   if (nContours)
      C = new centros[nContours];
   else
      return;

   if (!C)
      return;

   for (i = 0; i < nContours; ++i)
   {
      mo = moments (contoursIn[i]);

      max = min = contoursIn[i][0].y;
      for (j = 1; j < contoursIn[i].size(); ++j)
      {
         val = contoursIn[i][j].y;
         if (val > max)
            max = val;
         else
            if (val < min)
               min = val;
      }
      cM = cv::Point2f (static_cast < float >(mo.m10 / (mo.m00 + 1e-5)),
                        static_cast < float >(mo.m01 / (mo.m00 + 1e-5)));

      C[i].P = cM;
      C[i].idx = i;
      C[i].height = max - min;
   }

   qsort (C, nContours, sizeof (centros), cmpCentros);

   contoursOut.clear ();
   for (i = 0; i < nContours; ++i)
   {
      vector < cv::Point > P;

      idx = C[i].idx;
      cout.flush();
      contoursOut.push_back (contoursIn[idx]);
   }

   delete[]C;
}

void normHuMoments(double *Hu)
{
   unsigned int i;

   for (i = 0; i < 7; ++i)
      Hu[i] = -1 * copysign (1.0,Hu[i]) *log10 (abs (Hu[i]) + 1e-8);
}

