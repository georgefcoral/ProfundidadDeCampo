#include <iostream>
#include <serialPOSIX.h>

using namespace std;

int main(int argc, char **argv)
{
    ssize_t n;
    char *d;
    char valor = 0;
    serialPOSIX comm1(115200);

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
        do
        {
            n = comm1.send(&valor, 1);
            if (n >= 0)
                break;
            cerr << "Error al enviar el valor " << valor << endl;
            usleep(10000);
        } while(true);
        
        cout << "Se enviaron " << n << " bytes: " << (int) valor<< endl;
        cout.flush();
        
        do
        {
            d = comm1.receive(1, &n);
            if (d != NULL)
                break;
            usleep(100000);
        } while (true);
        
        valor = *d;
            
        cout << "se recibio el valor  : "<< (int)valor << endl << endl;
            
            
        valor = (valor + 1) % 128;
            
        //sleep(2);
        
    } while (1);

    return 0;
}
