#include <iostream>
#include <serialPOSIX.h>
#include <cstring>
#include <cmath>
using namespace std;

string encode(int step){

    if(step > 0){//Forward Code
        return "2" + to_string(abs(step)) + "a";
    }
    if(step == 0){//Stop code
        return "0" + (string)"a";
    }

    if(step < 0){//Backward code
        return "1" + to_string(abs(step)) + "a";
    }
    return "0" + (string)"a";
}


int main(int argc, char **argv)
{
    ssize_t n;
    ssize_t n2;
    serialPOSIX comm1(115200);
    string str = "0100";
    char *buff = new char[str.length()];  
    char *data = new char[1]; 
    int epochs = 5;
    int steps[epochs] = {0,-15, 11,879,-785};
    int k = 0;//Its a counter. 
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
        str = encode(steps[k]);
        strcpy(buff, str.c_str());

        //Enviar Datos
        do{  
        n = comm1.send(buff);
        cout<<"Cadena "<<buff<<" enviada"<<endl;
        cout<<"El valor de n es: "<<n<<endl;
        if (n >= 0)
            break;
        usleep(10000);
        }while(1);

        //Recibir datos
        do{
            data = comm1.receive(1, &n2);
            // if(data!= NULL && n2>=0){                    
            //     cout<<"Se recibió un "<<data<<"!"<<endl;
            //     break;
            // }
        }while(1);

        k++;
    } while(k<epochs);

    delete [] buff;
    return 0;
}


