/*  
	Example1394_OpenCV.cpp - an example of the use of the Camera1394 class.

<one line to give the program's name and a brief idea of what it does.>
  
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
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <Camera1394.h>
#include <iostream>
#include <unistd.h> 

using namespace std;
using namespace cv;

int main()
{
	Camera1394 C0(0);
	int cont;
	Mat frame;

	C0.Print_Cameras_List();
	C0.Print_Camera_Info();
	C0.Setup (10, DC1394_VIDEO_MODE_640x480_MONO8, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_60, 10, MONO8);
	C0.Set_Filename_Base("ChuchitaBolseada");
	C0.Set_MaxFileCounter(10);
	C0.Set_FileSaveType(FST_NONE);
	C0.Set_CaptureInterval (0.5);
	C0.Get_Capture_Interval();
	C0.Start_Capture();

	namedWindow("Camara");
	for (cont=0;;cont++)
	{
		uint32_t idx;
		uFrame *uF;
	   frame = Mat::zeros(Size(640, 480), CV_8UC1);

		idx = C0.Get_Last_Captured_Frame_Index();
		uF = C0.Get_uFrame_Pointer (idx);
		memcpy(frame.data, uF->image, uF->Size);
#ifdef VERBOSE
		cout << "uFrame["<< uF->Index << "].Cols         = " << uF->Cols << endl;
		cout << "uFrame["<< uF->Index << "].Rows         = " << uF->Rows << endl;
		cout << "uFrame["<< uF->Index << "].Size         = " << uF->Size << endl;
		cout << "uFrame["<< uF->Index << "].Depth        = " << uF->Depth << endl;
		cout << "uFrame["<< uF->Index << "].Frame_Index  = " << uF->Frame_Index
		     << endl;
		cout << "uFrame["<< uF->Index << "].timestamp= " << uF->timestamp << endl;
#endif
      C0.Release_uFrame_Pointer(idx);
		imshow( "Camara", frame);
		int ex = waitKeyEx(30);
		cout<<"ex: "<<ex<<endl;
   	if (ex>= 0 )
	   	break;
	}

	C0.Finish_Capture();
	C0.FlushStorageBuffer();

	return 0;
}
