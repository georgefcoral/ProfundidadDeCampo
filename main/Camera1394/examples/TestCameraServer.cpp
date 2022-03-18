/*  
   TestCameraServer.cpp - a program that thest de CameraServer class. 
  
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

#include <CameraServer.h>
#include <iostream>
#include <unistd.h>

int main()
{
	CameraServer C0 (0, 36000, 50);
	float CI;

 	C0.Print_Cameras_List();
   C0.Print_Camera_Info();
   C0.Setup (10, DC1394_VIDEO_MODE_640x480_MONO8, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_15, 10);
   C0.Set_Filename_Base("Camera_Zero");
   C0.Set_MaxFileCounter(10);
   C0.Set_FileSaveType(FST_NONE);
   C0.Set_CaptureInterval (1);
   CI=C0.Get_Capture_Interval();
   C0.Start_Capture();

	C0.StartServer();

	C0.Finish_Capture();
   C0.FlushStorageBuffer();

	return 0;
}
