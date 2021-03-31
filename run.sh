#!/bin/dash
if [ -z "$1"]
then 
	echo "Argumentos vacios, proporcione:"
	echo "./run.sh [path input images] [path output images]"
	exit 0
fi


mkdir "$2" -p
echo "Directorio para salidas creado."
rm myFile.txt
ls "$1">>myFile.txt
echo "Lista de imagenes preparadas."
cat myFile.txt
make
./ejecutable