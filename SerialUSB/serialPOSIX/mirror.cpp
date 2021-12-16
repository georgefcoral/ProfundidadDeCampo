#include <iostream>
#include <serialPOSIX.h>

using namespace std;

int main(int argc, char **argv)
{
    ssize_t n;
    char c, *d;
    serialPOSIX comm1(115200);

    if (argc < 2)
    {
        cerr << "Error: Faltan argumentos" << endl;
        cerr << "Uso:" << endl << endl
             << "   mirror UART_COMM_FILE " << endl << endl
             << "   Donde UART_COMM_FILE es algo como /dev/tty[S,O][0-5]," 
             << "posiblemente." << endl << endl;
        return -1;
    }

    comm1.openSerial(argv[1]);

    do
    {
        d = comm1.receive(1, &n);
        if (!n || !d)
        {
            cerr << "No se pudieron leer datos." << endl;
            break;
        }
        else
        {
            c = toupper(*d);
            comm1.send(&c, 1);
        }
    } while (*d != 27);

    return 0;
}
