/*  

  	Example1394.cpp - an exmple that test the Camera1394 class. 
  
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

#include <Camera1394.h>
#include <iostream>
#include <unistd.h>

int main()
{
	size_t cnt = 0;
	float CI;
	Camera1394 C0(0);

	C0.Print_Cameras_List();
	C0.Print_Camera_Info();
	C0.Setup (10, DC1394_VIDEO_MODE_800x600_YUV422, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_15, 10);
	C0.Set_Filename_Base("First_Take");
	C0.Set_MaxFileCounter(600);
	C0.Set_FileSaveType(FST_ALL);
	C0.Start_Transmission();
	CI=C0.Get_Capture_Interval();
	C0.Set_CaptureInterval (0.5);
	for (cnt = 0; cnt < 600; cnt++)
	{
		C0.Process_Frame();
		usleep (CI);
	}
	C0.FlushStorageBuffer();
	C0.Stop_Transmission();

	return 0;
}
