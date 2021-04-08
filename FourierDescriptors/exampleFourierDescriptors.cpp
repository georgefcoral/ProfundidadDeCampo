#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <Mosaic.h>
#include <iostream>
#include <FourierDescriptor.h>

int main(int argc, char **argv)
{
	Mat frame, rgbFrame, output, imContours;
   FourierDescriptor FD;
	double pDesc = 0.;
	double reconError;

	if (argc < 2)
	{
	    cerr << "Faltan Parámetros:\n\n\tUso: FourierDescriptors <Nombre_de_la_imagen>  [pDescriptores]\n\n";
	    cerr << "El proceso aplica primero realiza una umbralización de la imágen para posteriormente..." << endl << endl; 
		exit (1);
    }

    frame = imread(argv[1], 0);
    if (argc>2)
        pDesc = atof(argv[2]);

    Mosaic M(Size(frame.cols, frame.rows), 1, 2, 8, 8, CV_8UC3);

    //Abrimos las ventanas para mostrar los resultados.
    namedWindow( "Contornos", 1 );

    cvtColor(frame, rgbFrame, COLOR_GRAY2RGB);
    M.setFigure (rgbFrame, 0, 0);

    threshold(frame, output, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

    FD.setContours(output);
    FD.computeDescriptors();
    reconError = FD.reconstructContours(pDesc);
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
