/*  
   TestRingCounter.cpp - A program that test the use of the RingCounter class.
	
    Copyright (C) 2010  Arturo Espinosa-Romero (arturoemx@gmail.com)
    Facultad de Matemáticas, Universidad Autónoma de Yucatán, México.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <cstdio>
#include <cstdlib>
#include "RingCounter.h" 

int main(int argc, char **argv)
{
	size_t n = argc < 2 ? 10 : (size_t)atoi(argv[1]);
	size_t inc = argc > 2 ? (size_t)atoi(argv[2]) : 2;
	RingCounter R(n);

	R = n/2;
	printf ("Incrementando!\n");
	for (size_t i = 0; i < n * 2; ++i, ++R++)
	{
		printf ("\t%lu\n", (size_t)R);
	}

	R = n/2;
	printf ("Decrementando!\n");
	for (size_t i = 0; i < n * 2; ++i, --R--)
	{
		printf ("\t%lu\n", (size_t)R);
	}

	R = n/2;
	printf ("Incrementando de %lu en %lu!\n", inc, inc);
	for (size_t i = 0; i < n * 2; ++i, R +=  inc)
	{
		printf ("\t%lu\n", (size_t)R);
	}

	R = n/2;
	printf ("Decrementando de %lu en %lu!\n", inc, inc);
	for (size_t i = 0; i < n * 2; ++i, R -=  inc)
	{
		printf ("\t%lu\n", (size_t)R);
	}

	return 0;
}
