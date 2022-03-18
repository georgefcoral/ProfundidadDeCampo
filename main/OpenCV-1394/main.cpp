#include <iostream>
#include <serialPOSIX.h>
#include <cstring>
#include <cmath>
#include <fstream> 
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <Camera1394.h>
#include <unistd.h> 

using namespace std;
using namespace cv;

/* Paso del motor, 
numero positivo la camara se aleja del objeto.
numero negativo la camara se acerca al objeto.
*/
const string stepping = "150";

/*# de imagenes a*/
int iter = 30;


int main(int argc, char **argv)
{
    Mat frame;
    ofstream MyFile("data.txt");
    serialPOSIX comm1(115200);
    Camera1394 C0(0);
    ssize_t n;
    ssize_t n2;
    namedWindow("Capturador");
    string str = "0100";
    int finish = 0;
    char *buff = new char[str.length()];  
    char *data = new char[1]; 
    
    					/*CAMERA CONFIG*/
    C0.Print_Cameras_List();
	C0.Print_Camera_Info();
	C0.Setup (10, DC1394_VIDEO_MODE_640x480_MONO8, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_60, 10, MONO8);
	C0.Set_Filename_Base("ChuchitaBolseada");
	C0.Set_MaxFileCounter(10);
	C0.Set_FileSaveType(FST_NONE);
	C0.Set_CaptureInterval (0.5);
	C0.Get_Capture_Interval();
	C0.Start_Capture();
    int k = 0;//Its a counter
    string received = "";
    if (argc < 2)
    {
        cerr << "Error: Faltan argumentos" << endl;
        cerr << "Uso:" << endl << endl
             << "   test1 UART_COMM_FILE" << endl << endl
             << "   Donde UART_COMM_FILE es algo como /dev/tty[S,O][0-5]," 
             << "posiblemente." << endl << endl;
        return -1;
    }


    if (!comm1.openSerial(argv[1]) )
    {
        cerr << "Error al abrir e puerto " << argv[1] << endl;
        exit(1);
    }

	/*Envio de datos a Motor y cambio de cuadros*/
    do
    { 


        uint32_t idx;
		uFrame *uF;
        frame = Mat::zeros(Size(640, 480), CV_8UC1);
        //str = encode(steps[k]);
        strcpy(buff, stepping.c_str());
        
        /*Empieza a enviar Datos*/
        do{  
        n = comm1.send(buff);
        cout<<"Cadena "<<buff<<" enviada"<<endl;

        if (n >= 0)
            break;
        
        }while(1);
        usleep(1000);
        /*Termina enviar datos*/

        /*Recibe datos*/
        received = "";
        do{
            data = comm1.receive(1, &n2);
            if(data!= NULL && n2>=0){                    
                received += data[0];
                if((int)data[0] == 0)
                    finish = 1;
            }
            if(finish == 1){
                finish = 0;
                break;
            }
        }while(1);
        /*TERMINA RECIBIR DATOS*/

        /*Empieza Captura*/
		idx = C0.Get_Last_Captured_Frame_Index();
		uF = C0.Get_uFrame_Pointer (idx);
		memcpy(frame.data, uF->image, uF->Size);
#ifdef VERBOSE
		cout << "uFrame["<< uF->Index << "].Cols         = " << uF->Cols << endl;
		cout << "uFrame["<< uF->Index << "].Rows         = " << uF->Rows << endl;
		cout << "uFrame["<< uF->Index << "].Size         = " << uF->Size << endl;
		cout << "uFrame["<< uF->Index << "].Depth        = " << uF->Depth << endl;
		cout << "uFrame["<< uF->Index << "].Frame_Index  = " << uF->Frame_Index
		     << endl;
		cout << "uFrame["<< uF->Index << "].timestamp= " << uF->timestamp << endl;
#endif
      C0.Release_uFrame_Pointer(idx);
		imshow( "Capturador", frame);
        //imwrite("./outputs/"+to_string(k)+".jpg",frame);
		int ex = waitKeyEx(100);
   	    if (ex>= 0 )
	   	    break;
        MyFile <<k<<","<<received<<endl;
        cout<<"Iteracion "<< k <<" con distancia : "<<received  <<" mm."<<endl;
        k++;
        usleep(100);
    } while(k<iter);



    // Close the file
    
    MyFile.close();
    delete [] buff;
    C0.Finish_Capture();
	C0.FlushStorageBuffer();
    return 0;
}


