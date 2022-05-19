#ifndef __FRAMEDATA__
#define __FRAMEDATA__

#include "opencv2/opencv.hpp"
#include <vector>
#include <cstring>
#include <string>
#include <momHu.h>

using namespace std;
using namespace cv;
/*!
\struct frameData
\brief Esta Clase contiene cada uno de los atributos necesarios para poder realizar
   el trabajo de tesis que involucra trabajar con imágenes enfocadas y desenfocadas.
*/
struct frameData
{
   /*!
   \var string fileName
   \brief contiene el nombre del conjunto de archivos a procesar.
   */
   string fileName;
   /*!
   \var Mat Image
   \brief Imágenes de entrada a procesar.
   */
   Mat Image;
   /*!
   \var Mat thImage
   \brief Imágenes de entrada a procesar umbralizadas.
   */
   Mat thImage;
   /*!
   \var vector < vector < Point > >contours;
   \brief Contornos de la imagen de entrada, necesarios para realizar
      la segmentación.
   */
   vector < vector < Point > >contours;
   /*!
   \var vector < Moments > mu;
   \brief Momentos invariantes a escala y rotación .
   */
   vector < Moments > mu; //momentos.
   /*!
   vector <momHu> momentsHu;
   \brief Momentos de hu necesarios para encontrar correspondencias.
   */
   vector <momHu> momentsHu;//momentos de Hu.
   /*!
   \var vector < Point2f > mc;
   \brief Centros de masa de cada objeto.
   */
   vector < Point2f > mc; //Centros de Masa.
   /*!
   \var vector <float> areas;
   \brief Areas de cada uno de los objetos segmentados.
   */
   vector <float> areas;
   
   /*!
   \var vector <float> perimetros;
   \brief Perimetros de cada uno de los objetos segmentados.
   */
   vector <float> perimetros;
   
   
   
   
   /*!
      \fn frameData()
      \brief Constructor de inicialización por defecto, inicializa los atributos vacíos. 
   */
   frameData()
   {
   }

   /*!
      \fn frameData(const frameData &F)
      \brief Constructor de copia.
      \param frameData &F  
   */
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
      perimetros = F.perimetros;
   }

   /*!
      \fn void operator = (const frameData &F)
      \brief Sobre carda del operador = para comparar dos cuadros.
      \param frameData &F  
   */
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
      perimetros = F.perimetros;
   }
};

#endif
