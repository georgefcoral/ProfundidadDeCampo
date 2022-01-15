#include <iostream>
#include <cstring>

using namespace std;



int main()
{
   char Cadena[] = "1.23 12 1233 mama 23";
   char buff[255], *salida, *tmp;
   int cont = 0;


   cout << "Vamos a parsear la cadena \"" << Cadena << "\"." << endl << endl; 
   salida = strtok_r(Cadena, " ", &tmp);
   while (salida != 0)
   {
      strncpy(buff, salida, 254);
      cout << "El token Numero " << cont << " es " << salida << endl;
      salida = strtok_r(0, " ", &tmp);
      cont++;
   }
   return 0;

}
