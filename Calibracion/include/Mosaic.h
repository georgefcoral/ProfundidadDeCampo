#ifndef __CV_MOSAIC__
#define __CV_MOSAIC__

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;
/*!
\class Mosaic
\brief Esta clase permite el despliegue de un mosaico de imágenes en una sola ventana. Esto es útil, entre otras cosas, para evitar tener múltiples ventanas abiertas al mismo tiempo.
*/
class Mosaic
{
    /*!
    \var Mat Frame
    \brief Un objeto de la clase Mat, que sirve para almacenar el mosaico.
    */
    Mat frame;

    /*!
    \var Mat **figures
    \brief Un arreglo de objetos Mat, que servira para definir ROIs (region of interest) asociada a cada una de la imágenes que componen el mosaico.
    */
    Mat **figures;

    /*!
    \var int fRows
    \brief El número de renglones en cada una de las imágenes que componen el mosaico.
    */
    int fRows;

    /*!
    \var int fCols
    \brief El número de columnas en cada una de las imágenes que componen el mosaico.
    */
    int fCols;

    /*!
    \var int vSep
    \brief La separación vertical entre cada elemento del mosaico.
    */
    int vSep;

    /*!
    \var int hSep
    \brief La separación horizontal entre cada elemento del mosaico.
    */
    int hSep;

  public:
  
    /*!
    \fn Mosaic(Size figSize, int r, int c, int vs = 5, int hs = 5, int type = CV_8UC1)
    \brief Constructor de la clase mosaico.
    \param figSize un objeto de la clase figsize que almacena el numero de elementos que formaran el mosaico.
    \param r El número de renglones en cada una de las imágenes que componen el mosaico.
    \param c El número de columnas en cada una de las imágenes que componen el mosaico.
    \param vs La separación vertical entre cada elemento del mosaico.
    \param hs La separación horizontal entre cada elemento del mosaico.
    \param type El tipo de datos de la matriz (default CV_8UC1).
    */
    Mosaic(Size figSize, int r, int c, int vs = 5, int hs = 5, int type = CV_8UC1)
    {
        int rows, cols, ulFig_r, ulFig_c, i, j;

        fRows = r;
        fCols = c;
        vSep = vs;
        hSep = hs;
        rows = figSize.height * fRows + vSep * (fRows + 1);
        cols = figSize.width * fCols + hSep * (fCols + 1);
        frame = Mat::ones (rows, cols, type) * 255;
        figures = new Mat *[fRows];
        figures[0] = new Mat [fRows * fCols];
        for (int i=1;i < fRows;++i)
            figures[i] = &(figures[0][i * fCols]);

        ulFig_r = vSep;
        for (i = 0; i < fRows; ++i, ulFig_r += figSize.height)
        {
            ulFig_c = hSep;
            for (j = 0; j < fCols; ++j,ulFig_c += figSize.width)
            {
                figures[i][j] = frame (Range(ulFig_r+i*vSep,ulFig_r+i*vSep+figSize.height), Range(ulFig_c+j*hSep,ulFig_c+j*hSep+figSize.width));
            }
        }
    }
    
    /*!
    \fn void setFigure(Mat &im, int r, int c)
    \brief Asocia la figura im, al elemento con cordenadas (r,c) del mosaico.
    \param im Objeto que contiene la figura a ser asociada
    \param r, c Las coordenadas del elemento del mosaico al que se le va a asociar la figura.
    */
    void setFigure(Mat &im, int r, int c)
    {
        double scaleX, scaleY;

        if (r < 0 || r >= fRows || c < 0 || c >= fCols)
        {
            cerr << "Error: índices a figura fuera de rango" << endl;
            return;
        }
        scaleX = (double)figures[r][c].cols / im.cols;
        scaleY = (double)figures[r][c].rows / im.rows;
        if (scaleX != 1. || scaleY != 1.)
        {
            Mat tmp;
            resize(im, tmp, Size(), scaleX, scaleY, INTER_AREA);
            tmp.copyTo(figures[r][c]);
        }
        else
            im.copyTo(figures[r][c]);
    }
  
    /*!
    \fn void initFigure (int r, int c, Scalar val)
    \brief Inicializa un elemento del mosaico con un valor escalar.
    \param r, c Las coordenadas del elemento a incializar.
    \param val El valor con que se va a inicializar el elemento del mosaico.
    */
    void initFigure (int r, int c, Scalar val)
    {
        if (r >= 0 && r < fRows && c >= 0 && c < fCols)
        {
            Mat tmp(Size(frame.rows, frame.cols),frame.type(),val);
            tmp.copyTo (figures[r][c]);
        }
    }
  
    /*!
    \fn int getMosaicRows ()
    \brief Regresa el número de renglones que tiene el mosaico.
    \return El número de renglones que tiene el mosaico.
    */
    int getMosaicRows ()
    {
        return frame.rows;
    }
  
    /*!
    \fn int getMosaicCols ()
    \brief Regresa el número de columnas que tiene el mosaico.
    \return El número de columnas que tiene el mosaico.
    */
    int getMosaicCols ()
    {
        return frame.cols;
    }
  
    /*!
    \fn void show(const char *name)
    \brief Muestra ek mosaico en la ventana asociada con el nombre name
    \param name El nombre de la ventana en donde se mostrará el mosaico.
    */
    void show(const char *name)
    {
        
        imshow(name, frame);
    }
};

#endif
