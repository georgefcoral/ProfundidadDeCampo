/*  
    Camera1394.cpp - The methods of the class Camera1394, to control
	 the capture of images from a IEEE-1394 Camera.
  
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

#include <cstdlib>
#include <unistd.h>
#include <Camera1394.h>

void Camera1394::Init_Camera ()
{
		camera = 0;
		RB = 0;
		CamSetup = false;
		CamInit= false;
		CaptureFlag = false;
		capture_thread = 0;
		TransStatus = DC1394_OFF;
		camera_list = 0;
}

void Camera1394::Init_Camera (uint32_t n)
{
	dc1394error_t err;
	pthread_mutex_t recmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;


	pthread_mutex_init(&dev1394_mutex, NULL);
	CamReg_mutex = recmutex;

	Init_Camera();
	if (!dev1394)
	{
		pthread_mutex_lock (&dev1394_mutex);
		dev1394 = dc1394_new();
		pthread_mutex_unlock (&dev1394_mutex);

	}	
	pthread_mutex_lock (&dev1394_mutex);
	err = dc1394_camera_enumerate (dev1394, &camera_list);
	pthread_mutex_unlock (&dev1394_mutex);
	DC1394_ERR_CLN (err,cleanup_and_exit(), "Failed to enumerate cameras");

	if (n >= camera_list->num)
	{
		dc1394_log_error("Camera not found.");
		return;
	}
	pthread_mutex_lock (&dev1394_mutex);
	camera = dc1394_camera_new (dev1394, camera_list->ids[n].guid);
	pthread_mutex_unlock (&dev1394_mutex);
	if (!camera)
	{
		dc1394_log_error("Failed to initialize camera with guid %llx", camera_list->ids[0].guid);
		exit (1);
	}
	#ifdef VERBOSE		
	std::cout << "Se obtuvo la camara\n";
	#endif
	RB = 0;
	pthread_mutex_lock (&CamReg_mutex);
	CameraRegister.push_back (this);
	#ifdef VERBOSE
	std::cout << "Hay " << CameraRegister.size() << " Camaras registradas\n";
	#endif
	pthread_mutex_unlock (&CamReg_mutex);

	{
		action.sa_handler =  SigHandler_trampoline;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		sigaction (SIGINT, NULL, &oldaction);
		if (oldaction.sa_handler == SIG_DFL)
			sigaction (SIGINT, &action, &oldaction);
	}
	CamInit= true;
}

Camera1394::~Camera1394()
{
	if (CamInit)
	{
		if (CaptureFlag)
			Finish_Capture();
		dc1394_video_set_transmission(camera, DC1394_OFF);
		dc1394_capture_stop(camera);
		dc1394_camera_free(camera);
		dc1394_camera_free_list (camera_list);
		pthread_mutex_lock (&CamReg_mutex);
	 	remove (CameraRegister.begin(), CameraRegister.end(), this);
		#ifdef VERBOSE
 		std::cout << "Hay " << CameraRegister.size() << " Camaras registradas\n";
		#endif	
		if (CameraRegister.empty())
		{
			sigaction (SIGINT, &oldaction, NULL);
			pthread_mutex_lock (&dev1394_mutex);
			dc1394_free (dev1394);
			pthread_mutex_unlock (&dev1394_mutex);
		}
		pthread_mutex_unlock (&CamReg_mutex);

	}
}	

int Camera1394::Setup(uint32_t rbs, dc1394video_mode_t vMode,  dc1394speed_t isoSpeed, dc1394framerate_t fRate, int n, iFormat frmt)
{
	dc1394error_t err;

	if (!CamInit)
		return -1;

	err=dc1394_video_set_operation_mode(camera, DC1394_OPERATION_MODE_1394B);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not set operation mode.");
	err=dc1394_video_set_iso_speed(camera, isoSpeed);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not set iso speed");
	
	Set_VideoMode (vMode);
	Set_FrameRate (fRate);

	err=dc1394_capture_setup(camera, n, DC1394_CAPTURE_FLAGS_DEFAULT);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera\n");

	RB = new StoreFrameBuffer(rbs, frmt);
	Set_CaptureInterval();
	CamSetup = true;

	return 0;
}

void Camera1394::Set_CaptureInterval()
{
	dc1394framerate_t Fr;
	dc1394_video_get_framerate (camera, &Fr);
	dc1394_framerate_as_float (Fr, &Capture_Interval);
	Capture_Interval = 1.0e6/Capture_Interval;
}
void Camera1394::Set_CaptureInterval(float fact)
{
	dc1394framerate_t Fr;
	dc1394_video_get_framerate (camera, &Fr);
	dc1394_framerate_as_float (Fr, &Capture_Interval);
	Capture_Interval = fact * 1.0e6 / Capture_Interval;
}

void Camera1394::cleanup()
{
	if (CamInit)
	{
		if (CaptureFlag)
			Finish_Capture();

		dc1394_video_set_transmission(camera, DC1394_OFF);
  	   dc1394_capture_stop(camera);
  	   dc1394_camera_free(camera);
		dc1394_camera_free_list (camera_list);

		pthread_mutex_lock (&CamReg_mutex);
	 	remove (CameraRegister.begin(), CameraRegister.end(), this);
	
		if (CameraRegister.empty())
		{
		#ifdef VERBOSE
			std::cerr << "[cleanup]Restoring SIGINT handler\n";
			std::cerr.flush();
		#endif
			sigaction (SIGINT, &oldaction, NULL);
		}
		pthread_mutex_unlock (&CamReg_mutex);
	}	
	Init_Camera();
	#ifdef VERBOSE
		std::cerr << "[cleanup]finish cleanup\n";
		std::cerr.flush();
	#endif
}

void Camera1394::cleanup_and_exit()
{
	cleanup_all();
	exit(1);
}

void Camera1394::cleanup_all()
{
	 std::vector<Camera1394 *>::iterator cam;

	pthread_mutex_lock (&CamReg_mutex);
	 for (cam=CameraRegister.begin();cam != CameraRegister.end();++cam)
	{
#ifdef VERBOSE
	std::cerr << "[Cleanup_all] Cleaninig up one camera" << std::endl;
	std::cerr.flush();
#endif
    	(*cam)->cleanup ();
#ifdef VERBOSE
	std::cerr << "[Cleanup_all] One camera has been cleaned up" << std::endl;
	std::cerr.flush();
#endif
	}
#ifdef VERBOSE
	std::cerr << "[Cleanup_all] About to clear CameraRegister" << std::endl;
	std::cerr.flush();
#endif
	 CameraRegister.clear();
	 pthread_mutex_unlock (&CamReg_mutex);
	std::cerr << "[Cleanup_all] CameraRegister has bean cleared" << std::endl;
}

int Camera1394::Start_Transmission()
{
	dc1394error_t err;

	if (!CamSetup || TransStatus == DC1394_ON)
		return -1;

	err=dc1394_video_set_transmission (camera,DC1394_ON);
	DC1394_ERR_CLN_RTN (err,cleanup_and_exit(),"Could start transmission\n");

	TransStatus = DC1394_ON;

	#ifdef VERBOSE		
	std::cout << "Se inicio la transmision.\n";
	#endif

	return 0;
}

int Camera1394::Stop_Transmission()
{
	dc1394error_t err;

	if (!CamSetup || TransStatus == DC1394_OFF)
		return -1;

	err=dc1394_video_set_transmission (camera,DC1394_OFF);
	DC1394_ERR_CLN_RTN (err,cleanup_and_exit(),"Could not stop trasnmission\n");
	TransStatus = DC1394_OFF;


	#ifdef VERBOSE		
	std::cout << "Se termino la transmision.\n";
	#endif

	return 0;
}

int Camera1394::Process_Frame()
{
	dc1394error_t err;
	dc1394video_frame_t *frame;

	if (!CamSetup)
		return -1;

	#ifdef VERBOSE		
	std::cout << "Procesando un frame.\n";
	#endif

	err=dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not capture a frame\n");
	#ifdef VERBOSE		
	std::cout << "Se capturo un frame.\n";
	#endif

	RB->Queue (frame);
	#ifdef VERBOSE		
	std::cout << "Se encolo un frame.\n";
	#endif

	dc1394_capture_enqueue (camera, frame);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not enqueue a frame\n");
	#ifdef VERBOSE		
	std::cout << "Se regreso un frame.\n";
	#endif

	return 0;
}

int Camera1394::Process_Next_Stored_Frame()
{
	if (!CamSetup)
		return -1;
	RB->Dequeue();
	return 0;
}

void Camera1394::FlushStorageBuffer()
{
	if (!CamSetup)
		return;
	RB->Flush();
}

void Camera1394::Print_Cameras_List()
{
	if (!CamInit)
		return;
	
	std::cout << "There are a total of " << camera_list->num 
	          << " cameras.\n";
	for (uint32_t i=0;i<camera_list->num;++i)
	{
		std::cout <<  	"Camera :" << i
		          << "\n  GUID :" << camera_list->ids[i].guid
		          << "\n  unit :" << camera_list->ids[i].unit << std::endl;
	}
	std::cout << std::endl;
}

void Camera1394::Set_Filename_Base(const char *s)
{

	if (CamSetup)
		RB->Set_Filename_Base(s);
#ifdef VERBOSE
	else
		std::cerr << "Could not set StoreRingBuffer Filename_Base: Camera not setup\n";
#endif
}

void Camera1394::Set_FileSaveType (FileSaveType fs)
{
	if (RB)
		RB->Set_FileSaveType (fs);
}

void Camera1394::Set_MaxFileCounter (uint32_t n)
{
	if (RB)
		RB->Set_MaxCounter(n);
}
void Camera1394::SigIntHandler (int n)
{
	 std::vector<Camera1394 *>::iterator cam;

	pthread_mutex_lock (&CamReg_mutex);
	 for (cam=CameraRegister.begin();cam != CameraRegister.end();++cam)
    	(*cam)->cleanup ();
	pthread_mutex_unlock (&CamReg_mutex);

	exit(1); 
}

void Camera1394::Start_Capture()
{
	if (CamInit && CamSetup && !capture_thread)
	{
		CaptureFlag = true;
		if (pthread_create (&capture_thread, NULL, Capture_Thread, (void *)this))
		{
			std::cerr << "Could not Create Capture thread\n";
			std::cerr.flush();
			cleanup_and_exit();
		}
	}
}


void Camera1394::Finish_Capture()
{
	if (capture_thread)
	{
		CaptureFlag = false;
		pthread_join (capture_thread, 0);
		capture_thread = 0;
	}
}

float Camera1394::Get_Video_FrameRate()
{
	//dc1394error_t error;
	dc1394framerate_t FrameRate;
	float fr;


//	error = 
		dc1394_video_get_framerate (camera, &FrameRate);
//	error = 
		dc1394_framerate_as_float (FrameRate, &fr);
	return fr;
}

void Camera1394::Get_Frame_Size (uint32_t &Rows, uint32_t &Cols)
{
//	dc1394error_t error;
	dc1394video_mode_t video_mode;

//	error = 
		dc1394_video_get_mode	(camera, &video_mode);
//	error = 
		dc1394_get_image_size_from_video_mode (camera, video_mode, &Cols, &Rows);

}

void *Capture_Thread (void *arg)
{
	Camera1394 *obj = (Camera1394 *)arg;

	obj->Start_Transmission();
	while (obj->CaptureFlag)
	{
		obj->Process_Frame();
		usleep (obj->Capture_Interval);
	}
	obj->Stop_Transmission ();
	pthread_exit(NULL);
}

dc1394_t *Camera1394::dev1394;

struct sigaction Camera1394::oldaction;

std::vector <Camera1394 *> Camera1394::CameraRegister;

