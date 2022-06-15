#ifndef __OBJTRACKER__
#define __OBJTRACKER__

#include <iostream>
#include <cfloat>
#include <sys/timeb.h>
#include <string>
#include <vector>



//Esta enumeración contiene los posibles estados que pueden ser asociados
//a un objeto:
//
// NOTDEFINED -> Objeto que no está siendo seguido.
// DEFINED -> Objeto siendo actualmente seguido
// MISSING -> Objeto que estaba siendo seguido no se encontró.
typedef enum {NOTDEFINED=0, DEFINED, MISSING} objStatus;

// This struct stores and index to refer to other objects in the table,
// and also keeps a distance value.
struct tabIdx
{
   int idx;
   double distance;
   tabIdx()
   {
      idx = -1;
      distance = std::numeric_limits<double>::infinity();
   }
};

struct trackedObj
{
    objStatus status; //> El estado actual del objeto.
    
    vector<tabIdx> next, prev; //> Índices al instancias previas y siguientes del objeto.

    int wCounter; //> Contador que indica el numero de iteraciónes que deben ocurrir para hacer que un objeto con estado MISSING pase a ser un objeto no definido (NOTDEFINED).
    
    trackedObj(int wC=-1)
    {
        next = prev = -1;
        status = NOTDEFINED;
        wCounter = wC;
    }
    
    trackedObj (const trackedObj &o)
    {
        next = o.next;
        prev = o.prev;
        status = o.status;
        wCounter = o.wCounter;
    }
    ~trackedObj()
    {
      next.clear();
      prev.clear();
    }
    
    virtual double Distance (const trackedObj &o) {return 0;};
    
    virtual std::string repr(){return std::string();};
};

/*!
 \struct temporalObjsMem
 \brief this class allows to keep track of the state of a number of objects during a determinate number of timesteps. The class defines a 2-dimensional array where the status of a tracked object is stored.
*/
template <typename Ob>
struct temporalObjsMem
{
    /*!
    1D Array of object of type Ob. Used to store the trackeed objects. The rows correspond to objects observed at a particular time, each column correspond to a particular object.
    */
    Ob **Table;
    unsigned int maxElements; //Maximum number of different element that can be dealt simultanously.
    unsigned int maxSeq; //Queue size.
    long long *tStamps; //> Timestamps. As many as maxSeq
    unsigned int idx; //> Index to the current row in the table.
    unsigned int waitingCounter; //> The number of iterarions an object with MISSING status will stay in that state before being set as NOTDEFINED
    double distThr; //> Distance threshold. This threshold is used to distinguish when two object are similar enough or not.
    bool cleanRow;//> True when the table is clean and when all objects in the idx-i row are NOTDEFINED
    
    //Initialize an empty object array.
    temporalObjsMem (int wc=10)
    {
        Table = 0;
        tStamps = 0;
        idx = 0;
        maxElements = maxSeq = 0;
        distThr = FLT_MAX;
        waitingCounter = wc;
        cleanRow = true;
    }
    
    /*!
     \func temporalObjsMem(int mEl, int mSq, double dThr = FLT_MAX) //constructor.
     /brief temporalObjsMem constructor, initializes a temporal object array with 
     */
    temporalObjsMem(int mEl, int mSq, int wc, double dThr = FLT_MAX)
    {
        Table = 0;
        tStamps = 0;
        maxElements = mEl;
        maxSeq = mSq;
        idx = 0;
        waitingCounter = wc;
        distThr = dThr;
        cleanRow = true;
        if (maxElements > 0 && maxSeq > 0)
        {
            unsigned int i, j, tama;
            Ob *apuFV;
            
            tama = maxElements * maxSeq;
            Table = new Ob *[maxSeq];
            Table[0] = new Ob[tama];

            apuFV = Table[0] + maxElements;
            for (i = 1; i < maxSeq; ++i, apuFV += maxElements)
            {
                Ob *apuFV2;
                apuFV2 = apuFV;
                for (j=0;j<maxElements;++j, ++apuFV2)
                    apuFV2->wCounter = waitingCounter;
                Table[i] = apuFV; 
            }
            tStamps = new long long[maxSeq];
        }
    }
    
    /*!
    \func ~temporalObjsMem()
    \brief Object destructor.
    */
    ~temporalObjsMem()
    {
        if (!Table)
            return;
        delete[] Table[0];
        delete[] Table;
        delete[] tStamps;
    }

    /*!
    \func void addDescriptors(std::vector<Ob> &o, long long ts)
    \brief This function receive a vector of objects that were detected on a frame, to be added to the temporal table
         Those object will be added to the next row available in the table, and a relationship between each new added
         object and objects stored in the table previous row will be established. The relationship  is determined in
         terms the distance measure; if the distance measure between two objects in adyacent rows is less that distance
         threshold distThr the relationship is stablished between them. This relationship is expressed by pointers between objects (prev, next).
    \param std::vector<Ob> &o Vector of object that will be inserted into the table.
    \param long long ts timestamp indicating the time the object were captured. 
    */
    void addDescriptors(std::vector<Ob> &o, long long ts)
    {
        unsigned int i, j, idxPrev, best, nOb;
        double tmp, d;

        // Determine the index of the previuos row in the table. 
        // In case idx == 0, the index to the last table row is assigned to idxPrev.
        // idPrex = idx-1 otherwise.
        idxPrev = idx == 0 ? maxSeq - 1:  idx - 1;

        tStamps[idx] = ts;

        //Initialize the table row where the new objects will be inserted.

        cleanRow = true;
        //This loop initialize the idx table row.
        for (i=0;i<maxElements;++i)
        {
            Table[idx][i] = Ob();
            Table[idx][i].status = NOTDEFINED;
            Table[idx][i].wCounter = waitingCounter;
            Table[idx][i].next = Table[idx][i].prev= -1;
            if (Table[idxPrev][i].status != NOTDEFINED)
               cleanRow = false;
        }

        nOb = o.size();

        if (cleanRow)
        {
            // Copies the object in the frame to the idx table row.
            for (i=0;i<nOb;++i)
            {
               Table[idx][i] = o[i];
               Table[idx][i].next = Table[idx][i].prev = -1;
               Table[idx][i].status = DEFINED;
            }
            // Clean up the rest of the row.
            for (;i < maxElements;++i)
            {
               Table[idx][i].status = NOTDEFINED;
               Table[idx][i].wCounter = waitingCounter;
               Table[idx][i].next = Table[idx][i].prev= -1;
            }
        }
        else
        {
           if (!nOb) // There are not detected objects in the current frame.
           {
              for (i = 0;i < maxElements; ++i)
              {
                  if (Table[idxPrev][i].status != NOTDEFINED)
                  {
                     Table[idx][i].status = MISSING;
                     Table[idx][i].wCounter = Table[idxPrev][i].status == DEFINED ? waitingCounter :Table[idxPrev][i].wCounter - 1;
                     Table[idx][i].prev = i;
                     Table[idx][i].next = -1;
                     Table[idxPrev][i].next = i;
                  }
               }
           }
           else
           {
              int Matches[maxElements], iMatches[nOb];
              unsigned int nMatched = 0;

              for (i = 0;i < maxElements; ++i)
                 Matches[i] = -1;
              for (i = 0;i < nOb; ++i)
                 iMatches[i] = -1;

              for (i=0;i<nOb;++i)
              {
                  d = -1.;
                  for (j = 0;j < maxElements; ++j)
                     if (Table[idxPrev][i].status != NOTDEFINED)
                        break;
                  d = o[i].Distance(Table[idxPrev][j]);
                  best = j;
                  for (;j < maxElements; ++j)
                  {
                     tmp = o[i].Distance(Table[idxPrev][j]);
                     if (tmp < d)
                     {
                        d = tmp;
                        best = j;
                     }
                  }
                  if (d >= 0 && d < distThr)
                  {
                     nMatched++;
                     Matches[best] = i;
                     iMatches[i] = best;
                  }
              }
              for (i = 0;i < maxElements; ++i)
              // This loop tries to find for each object stored in the previous row, the
              // one in the current row that is the closest. i is an index to the object
              // in the previouous row.
              {
                  // Here we validate that the element (idxPrev,i) in the table is one that has been defined
                  // (i.e. corresponds to a detected object on the previous frame), or is missing (has been
                  // detected, wasn't present in the last frame but is spected to show up eventually).
                  if (Table[idxPrev][i].status != NOTDEFINED)
                  {
                     // If  the minimum distance in less than a threshold, relate that object
                     // to the object 'i' in the previous frame.
                     if (Matches[i] != -1)
                     {
                        best = Matches[i];
                        Table[idx][i] = o[best];
                        Table[idxPrev][i].next = best;
                        Table[idx][i].prev = i;
                        Table[idx][i].status = DEFINED;
                        Table[idx][i].wCounter = waitingCounter;
                     }
                     else
                     {  //if there are no objects in o whose distance is below distThr
                        //The object stored in idxPrev should be marked as missing;
                        Table[idx][i] = Table[idxPrev][i];
                        Table[idx][i].status = MISSING;
                        Table[idx][i].wCounter =Table[idxPrev][i].wCounter-1;
                        Table[idxPrev][i].prev = i;
                        Table[idxPrev][i].next = -1;
                     }
                  }
              }
              //Add not matched objects to the table.
              j = 0;
              for (i = 0; i < nOb; ++i)
              {
                 if (nMatched != nOb && iMatches[i] != -1)
                 {
                     for (;j < maxElements; ++j) //Finds and empty slot in the table.
                        if (Table[idx][j].status == NOTDEFINED)
                        {
                            Table[idx][j] = o[i];
                            Table[idx][j].next = Table[idx][j].prev = -1;
                            Table[idx][j].status = DEFINED;
                            Table[idx][j].wCounter = waitingCounter;
                            nMatched++;
                            iMatches[i] = j;
                            break;
                        }
                     if (j >= maxElements)
                     {
                        std::cerr << "Error: se quedaron objetos sin registrar" << std::endl;
                        break;
                     }
                  }
               }
           }
           //Cleanup those object in the current frame who are missing, and their wait counter is 0
           for (j = 0; j < maxElements; ++j)
               if (Table[idx][j].status == MISSING && Table[idx][j].wCounter == 0)
               {
                   Table[idx][j].status = NOTDEFINED;
                   Table[idx][j].wCounter = waitingCounter;
               }
         }
#ifdef __VERBOSE__
        std::cout << "objs[" << idx << "] @ " << tStamps[idx] << " ms. = [";
        for (i = 0; i < maxElements; ++i)
        {
           if (Table[idxPrev][i].status == DEFINED)
           {
                std::cout << i << " : " << Table[idx][i].repr() << " {" << Table[idx][i].prev <<", " << Table[idx][i].next << "|" << Table[idx][i].Distance(Table[idxPrev][Table[idx][i].prev])<< "}";
            if (i != (maxElements - 1))
                std::cout << ", ";
           }
        }
        std::cout << "]" << std::endl;
        std::cout.flush();
#endif
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
                    case MISSING: std::cout << "M";
                };
            std::cout.setf(std::ios::hex);
            std::cout << Table[i][j].wCounter << " ";
        }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    int objsFrame(unsigned int idxFrame)
    {
      int cont = 0;
      unsigned int i;

      if (idxFrame >= maxSeq)
         return -1;
      for (i=0;i<maxElements;++i)
         if (Table[idxFrame][i].status == DEFINED)
            cont++;
      return cont;
    }
};


#endif
