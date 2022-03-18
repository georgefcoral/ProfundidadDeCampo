/*  
   Example1394_Process.cpp - an example of trhe use of the Camera1394 class.

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
	size_t cont = 0;
	Camera1394 C0(0);
	float CI;
	size_t idx;
	uFrame *uF;

	C0.Print_Cameras_List();
	C0.Print_Camera_Info();
	C0.Setup (10, DC1394_VIDEO_MODE_640x480_YUV411, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_15, 10);
	C0.Set_Filename_Base("Camera_Zero");
	C0.Set_MaxFileCounter(10);
	C0.Set_FileSaveType(FST_RINGBUFFER);
	C0.Set_CaptureInterval (1);
	CI=C0.Get_Capture_Interval();
	C0.Start_Capture();
	while (cont < 120)
	{
		idx = C0.Get_Last_Captured_Frame_Index();

		uF = C0.Get_uFrame_Pointer (idx);
		if (uF && uF->Size)
		{
			char buff[256];
			std::fstream fo;
			
			snprintf (buff,255,"FILE_%05lu.pnm", cont);

			fo.open(buff, std::ios::out);
			if (uF->format == MONO8)
				fo << "P5" << std::endl;
			else	
				fo << "P6" << std::endl;
			fo << "#Timestamp: " << uF->timestamp << std::endl;
			fo << "#Index: " << uF->Index << std::endl;
			fo << "#Frame Index: " << uF->Frame_Index << std::endl;
			fo << uF->Cols << " " << uF->Rows << "\n" << 255 << std::endl;
			fo.write((char *)uF->image, uF->Size);
			fo.close();
		}
		C0.Release_uFrame_Pointer(idx);
		cont++;
		usleep (CI);
	}

	C0.Finish_Capture();
	C0.FlushStorageBuffer();

	return 0;
}
