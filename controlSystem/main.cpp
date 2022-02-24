#include <iostream>
#include <serialPOSIX.h>
#include <cstring>
#include <cmath>
using namespace std;


//Esta función codifica cadena de caracteres 
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
    int finish = 0;
    char *buff = new char[str.length()];  
    char *data = new char[1]; 
    int iter = 5;
   // int steps[iter] = {0,-15, 11,879,-785};
    string stepsString[iter] ={"0","0", "0","0","0"};
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
        strcpy(buff, stepsString[k].c_str());

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
        cout<<"Distancia : "<<received  <<" mm."<<endl;
        k++;
        usleep(10000);
    } while(k<iter);

    delete [] buff;
    return 0;
}


