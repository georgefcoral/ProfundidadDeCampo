#ifndef __MOMHU__
#define __MOMHU__

#include <string>

struct momHu
{
   double mH[7];
   momHu()
   {
      memset(mH,0,7*sizeof(double));
   }
   momHu(const momHu &M)
   {
      memcpy(mH,M.mH,7*sizeof(double));
   }
   void operator = (const momHu &M)
   {
      memcpy(mH,M.mH,7*sizeof(double));
   }
};


#endif
