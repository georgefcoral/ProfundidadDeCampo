#ifndef __FEATURETRACKER__
#define __FEATURETRACKER__

#include <iostream>
#include <cfloat>
#include <sys/timeb.h>
#include <string>
#include <vector>
#include <math.h>



//Esta enumeración contiene los posibles estados que pueden ser asociados
//a un razgo:
//
// NOTDEFINED -> Feature que no está siendo seguido.
// DEFINED -> Feature siendo actualmente seguido
typedef enum {NOTDEFINED=0, DEFINED} featureStatus;

// This struct stores and index to refer to other features in the table,
// and also keeps a distance value.
struct tabIdx
{
   int idx;
   double distance;
   tabIdx()
   {
      idx = -1;
      distance = INFINITY;
   }
   tabIdx(int i, double d)
   {
      idx = i;
      distance = d;
   }

};

bool cmpTabIdx(const tabIdx &a, const tabIdx &b)
{
   return (a.distance < b.distance);
}

struct trackedFeature
{
    int ftId; //> Feature Id.
    featureStatus status; //> Feature actual state. This is usefull when we consider a table of potential features.
    
    std::vector<tabIdx> next, prev; //> Vector de índices al potenciales instancias previas y futuras del objeto.

    bool matched; //true if the first feature in the next std::vector points back to this features with its first feature.

    trackedFeature()
    {
        next.clear();
        prev.clear();
        matched = false;
        ftId = -1;
        status = NOTDEFINED;
    }
    
    trackedFeature (const trackedFeature &o)
    {
        next = o.next;
        prev = o.prev;
        status = o.status;
        matched = o.matched;
        ftId = o.ftId;
    }
    ~trackedFeature()
    {
      next.clear();
      prev.clear();
    }
    
    virtual double Distance (const trackedFeature &o) {return 0;};
    
    virtual std::string repr(){return std::string();};
};

/*!
 \struct temporalFeatureTable
 \brief this class allows to keep track of the state of a number of features during a determinate number of time. The class defines a 2-dimensional array where the status of a tracked feature is stored.
*/
template <typename Feat>
struct tempFeatureTable
{
    /*!
    1D Array of object of type Feat. Used to store the trackeed objects. The rows correspond to objects observed at a particular time, each column correspond to a particular object.
    */
    Feat **Table;
    unsigned int maxElements; //Maximum number of different element that can be dealt simultanously.
    unsigned int maxSeq; //Queue size.
    int idCounter;
    long long *tStamps; //> Timestamps. As many as maxSeq
    unsigned int idx; //> Index to the current row in the table.
    double distThr; //> Distance threshold. This threshold is used to distinguish when two object are similar enough or not.
    
    //Initialize an empty object array.
    tempFeatureTable ()
    {
        Table = 0;
        tStamps = 0;
        idCounter = 0;
        idx = 0;
        maxElements = maxSeq = 0;
        distThr = FLT_MAX;
    }
    
    /*!
     \func tempFeatureTable(int mEl, int mSq, double dThr = FLT_MAX) //constructor.
     /brief tempFeatureTable constructor, initializes a temporal object array with 
     */
    tempFeatureTable(int mEl, int mSq, double dThr = FLT_MAX)
    {
        Table = 0;
        tStamps = 0;
        idCounter = 0;
        idx = 0;
        maxElements = mEl;
        maxSeq = mSq;
        distThr = dThr;
        if (maxElements > 0 && maxSeq > 0)
        {
            unsigned int i, tama;
            Feat *apuFV;
            
            tama = maxElements * maxSeq;
            Table = new Feat *[maxSeq];
            Table[0] = new Feat [tama];

            apuFV = Table[0] + maxElements;
            for (i = 1; i < maxSeq; ++i, apuFV += maxElements)
               Table[i] = apuFV; 
            tStamps = new long long[maxSeq];
        }
    }
    
    /*!
    \func ~tempFeatureTable()
    \brief Object destructor.
    */
    ~tempFeatureTable()
    {
        if (!Table)
            return;
        delete[] Table[0];
        delete[] Table;
        delete[] tStamps;
    }

    /*!
    \func void addDescriptors(std::vector<Feat> &o, long long ts)
    \brief This function receive a std::vector of features that were detected on a frame, to be added to the temporal table
         Those object will be added to the next row available in the table, and a relationship between each new added
         object and objects stored in the table previous row will be established. The relationship  is determined in
         terms the distance measure; if the distance measure between two objects in adyacent rows is less that distance
         threshold distThr the relationship is stablished between them. 
         This relationship is expressed by pointers between objects stored in the std::vectore prev & next.
    \param std::vector<Feat> &ftv Vector of features that will be inserted into the table.
    \param long long ts timestamp indicating the time the object were captured. 
    */
    void addDescriptors(std::vector<Feat> &ftv, long long ts)
    {
        unsigned int i, j, idxPrev, nFt;
        double d;

        // Determine the index of the previuos row in the table. 
        // In case idx == 0, the index to the last table row is assigned to idxPrev.
        // idPrex = idx-1 otherwise.
        idxPrev = idx == 0 ? maxSeq - 1:  idx - 1;

        tStamps[idx] = ts;

        //Initialize the table row where the new objects will be inserted.

        //This loop initialize the idx table row.
        for (i=0;i<maxElements;++i)
            Table[idx][i] = Feat();

        nFt = ftv.size();

        if (nFt) // There are detected objects in the current frame.
        {
           for (i=0;i<nFt;++i)
           {
               Table[idx][i] = ftv[i];
               Table[idx][i].ftId = idCounter++;
               Table[idx][i].status = DEFINED;
               for (j = 0; j < maxElements; ++j)
               {
                     if (Table[idxPrev][j].status == DEFINED)
                     {
                        d = ftv[i].Distance(Table[idxPrev][j]);
                        if (d < distThr)
                        {
                           Table[idxPrev][j].next.push_back(tabIdx((int)i,d));
                           Table[idx][i].prev.push_back(tabIdx((int)j,d));
                        }
                     }
               }
           }
           //Sort the list of adyacent features
           for (i = 0;i < maxElements; ++i)
           {
               bool testA, testB;
               testA = testB = false;
               if (Table[idxPrev][i].status == DEFINED && Table[idxPrev][i].next.size() > 0 )
               {
                  sort(Table[idxPrev][i].next.begin(),Table[idxPrev][i].next.end(), cmpTabIdx);
                  testA = true;
               }
               if (Table[idx][i].status == DEFINED && Table[idx][i].prev.size() > 0)
               {
                  sort(Table[idx][i].prev.begin(),Table[idx][i].prev.end(), cmpTabIdx);
                  testB = true;
               }
               if (testA && testB)
               {
                  if (Table[idx][Table[idxPrev][i].next[0].idx].prev[0].idx == (int)i)
                     Table[idxPrev][i].matched = true;
               }
           }
        }
    }

    void incIdx()
    {
        idx = (idx+1) % maxSeq;
    }

   void printGrid()
    {
        unsigned int i, j;

        for (i=0;i<maxSeq;++i)
        {
            if (i == idx)
                std::cout <<"> ";
            else
                std::cout << "  ";
            for (j=0;j<maxElements;++j)
            {
                switch (Table[i][j].status)
                {
                    case NOTDEFINED: std::cout << "N"; break;
                    case DEFINED: std::cout << "D"; break;
                };
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    int featsFrame(unsigned int idxFrame)
    {
      int cont = 0;
      unsigned int i;

      if (idxFrame >= maxSeq)
         return -1;
      for (i = 0; i < maxElements; ++i)
         if (Table[idxFrame][i].status == DEFINED)
            cont++;
      return cont;
    }
};


#endif
