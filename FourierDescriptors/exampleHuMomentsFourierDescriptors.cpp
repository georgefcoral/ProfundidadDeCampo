#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <Mosaic.h>
#include <iostream>
#include <FourierDescriptor.h>

int main(int argc, char **argv)
{
	Mat frame, rgbFrame, output, imContours;
   FourierDescriptor FD;
	unsigned nDesc = 0;
	double reconError;

	if (argc < 2)
	{
	    cerr << "Faltan Parámetros:\n\n\tUso: exampleHuMomentsFourierDescriptors <Nombre_de_la_imagen>  [nDescriptores]\n\n";
	    cerr << "El proceso aplica primero realiza una umbralización de la imágen para posteriormente..." << endl << endl; 
		exit (1);
    }

    frame = imread(argv[1], 0);
    if (argc>2)
        nDesc = atoi(argv[2]);

    Mosaic M(Size(frame.cols, frame.rows), 1, 2, 8, 8, CV_8UC3);

    //Abrimos las ventanas para mostrar los resultados.
    namedWindow( "Contornos", 1 );

    cvtColor(frame, rgbFrame, COLOR_GRAY2RGB);
    M.setFigure (rgbFrame, 0, 0);

    threshold(frame, output, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

    FD.setContours(output);
    FD.computeDescriptor();

    //Aprovechamos que el objeto FD ya tiene encontró los contornos en la imagen, y los reusamos para calcular los momentos de Hu.
    {
      unsigned int i, j;
      vector <Moments> mu(FD.contourIn.size());
      vector <double[7]>huMoments(FD.contourIn.size());

      for (i=0;i<FD.contourIn.size();++i)
      {
         mu[i] = moments( FD.contourIn[i] );
         HuMoments(mu[i], huMoments[i]);

         cout << "Momentos invariantes de Hu del objeto " << i << " : " << endl << "["; 

         //Escalamos momentos de Hu.
         //(de acuerdo a: https://learnopencv.com/shape-matching-using-hu-moments-c-python/)
         for (j=0;j<7;++j)
         {
            huMoments[i][j] = -1 * copysign(1.0, huMoments[i][j]) * log10(abs(huMoments[i][j]));
            cout << huMoments[i][j];
            if (j < 6)
               cout << ", ";
            else
               cout << "]" << endl;
         }
      }
      cout << endl;
    }

    reconError = FD.reconstructContour(nDesc);
    cout << "El tamaño del contorno número " << 0 << " es igual a "
             << FD.contourOut[0].size() << " y el error de reconstruccion es " 
             << reconError << endl;
    plotContours(output, imContours, FD); 

    M.setFigure(imContours, 0, 1);

    M.show("Contornos");

    waitKeyEx( 0 );
    destroyAllWindows();

    return 0;
}
