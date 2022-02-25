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
numero positivo la camara se acerca al objeto.
numero negativo la camara se aleja del objeto.
*/
const string stepping = "200";

/*# de imagenes a*/
int iter = 10;


int main(int argc, char **argv)
{
    ofstream MyFile("data.txt");
    serialPOSIX comm1(115200);
    Camera1394 C0(0);
	int cont;
	Mat frame;
    ssize_t n;
    ssize_t n2;
    
    string str = "0100";
    int finish = 0;
    char *buff = new char[str.length()];  
    char *data = new char[1]; 
    
   // int steps[iter] = {0,-15, 11,879,-785};
    //string stepsString[iter] ={"100","10", "10","10","10"};
    
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


    do
    { 
        //str = encode(steps[k]);
        strcpy(buff, stepping.c_str());

        //Enviar Datos
        do{  
        n = comm1.send(buff);
        cout<<"Cadena "<<buff<<" enviada"<<endl;
        //cout<<"El valor de1 n es: "<<n<<endl;
        if (n >= 0)
            break;
        
        }while(1);
        usleep(10000);
        //Recibir datos
        //int countChars = 0;
        received = "";
        do{
            //Argumentos de función son n2,data
            data = comm1.receive(1, &n2);
           //cout<<"Data: "<<data<<" n2: "<<n2<<endl;
            if(data!= NULL && n2>=0){                    
                //cout<<"Se recibió un : "<<data[0]<<endl;
                received += data[0];
                if((int)data[0] == 0)
                    finish = 1;
            }
            if(finish == 1){
                finish = 0;
                break;
            }
           // cout<<"countChars: "<<countChars<<endl;
/*             if(finish == 1){
                cout<<"Holas";
                finish = 0;
                
            } */
            //countChars++;
        }while(1);
        cout<<"Iteracion "<< k <<" con distancia : "<<received  <<" mm."<<endl;
        MyFile <<k<<","<<received<<endl;
        k++;
        usleep(1000);
    } while(k<iter);
    // Close the file
    MyFile.close();
    delete [] buff;
    
    return 0;
}


