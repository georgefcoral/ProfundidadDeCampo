/*  

  	TestRingBuffer.cpp - a program that tests the ReingBuffer Class.
  
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

#include <iostream>
#include <cstdlib>
#include <RingBuffer.h>

int main(int argc, char **argv)
{
	int n = argc < 2 ? 10 : atoi(argv[1]);
	RingBuffer <int> R(10);



	for (int i = 0; i < n + 2 ; ++i)
	{
		int val = (int)random() % 50;
		
		std::cout << "Encolamos el valor: " << val << " y ... ";
		std::cerr << "La operacion ";
		if (R.Queue (val) < 0)
			std::cerr << "no";
		else
			std::cerr << "si";
		 std::cout << " fue un exito." << std::endl;
	}

	for (int i = 0; i < n + 2 ; ++i)
	{
		int val=0, err;

		err = R.Dequeue (val);
		std::cout << "Desencolamos el valor: " << val << " y ... ";
		std::cerr << "La operacion ";
		if (err < 0)
			std::cerr << "no";
		else
			std::cerr << "si";
		 std::cout << " fue un exito." << std::endl;
	}

	return 0;	
}


