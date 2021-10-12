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

struct trackedObj
{
    objStatus status; //> El estado actual del objeto.
    
    int next, prev; //> Índices al instancias previas y siguientes del objeto.

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
    unsigned int idx;
    unsigned int waitingCounter; //> The number of iterarions an object with MISSING status will stay in that state before being set as NOTDEFINED
    double distThr; //> Distance threshold. This threshold is used to distinguish when two object are similar enough or not.
    
    //Initialize an empty object array.
    temporalObjsMem (int wc=10)
    {
        Table = 0;
        tStamps = 0;
        idx=0;
        maxElements = maxSeq = 0;
        distThr = FLT_MAX;
        waitingCounter = wc;
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
    \brief Esta funcion recibe un vector de objetos que se encontraron para ser incorporador es la 
        This function inserts into the object table the objects stored in the vector of objects o. Those object will be added to the next row available in the table, and a relationship between the object and previous objects stored in the table will be made is the distance between them is smaller than distThresh. 
    \param std::vector<Ob> &o Vector of object that will be inserted into the table.
    \param long long ts timestamp indicating the time the object were captured. 
    */
    void addDescriptors(std::vector<Ob> &o, long long ts)
    {
        unsigned int i, j, idxPrev, best;
        double tmp, d;

        idxPrev = idx == 0 ? maxSeq - 1:  idx - 1;
        tStamps[idx] = ts;

        //Inicializamos el nuevo renglon
        for (i = 0;i < o.size() && i < maxElements; ++i)
        {    
            Table[idx][i] = o[i];
            Table[idx][i].wCounter = waitingCounter;
            Table[idx][i].status = DEFINED;
            Table[idxPrev][i].next = Table[idxPrev][i].prev= -1;
        }
        for (;i<maxElements;++i)
        {
            Table[idx][i].status = NOTDEFINED;
            Table[idx][i].wCounter = waitingCounter;
            Table[idxPrev][i].next = Table[idxPrev][i].prev= -1;
        }

        if (i < o.size())
            std::cerr << "Error: se quedaron objetos sin registrar" << std::endl;

        for (i = 0;i < maxElements; ++i)
        {
            if (Table[idxPrev][i].status == DEFINED || Table[idxPrev][i].status == MISSING)
            {
                    
                if (!o.size() && Table[idxPrev][i].status != NOTDEFINED)
                {
                    Table[idx][i].status = MISSING;
                    Table[idx][i].wCounter = Table[idxPrev][i].status == DEFINED ? waitingCounter :Table[idxPrev][i].wCounter - 1;
                    Table[idx][i].next = Table[idxPrev][i].prev;
                }
                else
                {
                    d = Table[idxPrev][i].Distance(Table[idx][0]);
                    best = 0;
                    for (j = 1; j < o.size(); ++j)
                    {
                        tmp = Table[idxPrev][i].Distance(Table[idx][j]);
                        if (tmp < d)
                        {
                            d = tmp;
                            best = j;
                        }
                    }
                    if (d < distThr)
                    {
                        Table[idxPrev][i].next = best;
                        Table[idx][best].prev = i;
                        if (Table[idxPrev][i].status != DEFINED)
                        {
                            Table[idx][best].status = DEFINED;
                            Table[idx][best].wCounter = waitingCounter;
                        }
                    }
                    else
                    {
                        Table[idx][best].status = MISSING;
                        Table[idx][best].wCounter =Table[idx][best].wCounter-1;
                        Table[idxPrev][i].next = Table[idxPrev][i].prev;
                    }
                }
            }
        }
        for (j = 0; j < maxElements; ++j)
            if (Table[idx][j].status == MISSING && Table[idx][j].wCounter == 0)
            {
                Table[idx][j].status = NOTDEFINED;
                Table[idx][j].wCounter = waitingCounter;
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
};


#endif
