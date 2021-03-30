#!/bin/dash
mkdir Outputs -p
echo "Directorio para salidas creado."
rm myFile.txt
ls Input>>myFile.txt
echo "Lista de imagenes preparadas."
cat myFile.txt
make
./ejecutable