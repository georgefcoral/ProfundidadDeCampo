/*  
	Camera1394.h - Header file that defines a class to control a IEEE-1394
	 camera.
  
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

#ifndef CAMERA1394_SYSTEM
#define CAMERA1394_SYSTEM

#include <dc1394/dc1394.h>
#include <StoreFrameBuffer.h>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <pthread.h>
#include <vector>
#include <algorithm>

/*!
	\class Camera1394
	\brief The Camera1394 is used to define objects that provide an interface
	       to work with IEEE-1394 cameras. it provides mean to work with more
	       than one camera at the same time.
*/
class Camera1394
{
	protected:
		/*! \var static dc1394_t *dev1394
		   \brief A pointer to a "context in wich the cameras are used.
		           it is an static variable, and all the objects instanciated
		           frome this clas will refer to it.
		*/
		static dc1394_t *dev1394;

		//! \var dc1394camera_t *camera
		//! \brief A pointer to the camera.
		dc1394camera_t *camera;

		//! \var dc1394camera_list_t *camera_list
		//! \brief a pointer to a camera list. It is initialized when the
		//!        object is initialized-
		dc1394camera_list_t *camera_list;

		//! \var struct sigaction oldaction.
		//! \brief This structure holds a pointer to the original SIGINT
		//!        interrupt handler. 
		static struct sigaction oldaction;

		/*! \var struct sigaction action.
		 \brief This structure holds a pointer to a SIGINT interrupt handler. 
		        when an SIGINT interruptio is issued to the program, all the 
		        cameras are stopped released, in order to have a clean exit.
		*/
		struct sigaction action;


		/*! \var static std::vector <Camera1394 *> CameraRegister
			 \brief A vector that stores the pointer of every Camera1394 object
			        instanciated.
		*/
		static std::vector <Camera1394 *> CameraRegister;

		//! \var dc1394switch_t TransStatus
		//! \brief This variable hold the transmition status (DC1394_ON)
		//!   or DC_1394 off.
		dc1394switch_t TransStatus;

		/*! \var float Capture_Interval
		    \brief The interval between shots. It is defined using 
		      void Camera1394::Set_CapureInterval(), in terms of the 
		      camera frame rate.
		*/
		float Capture_Interval;
		
		/*! \var StoreFrameBuffer *RB;
		     \brief A pointer of objects of the class StoreFrameBuffer,
		            which is a Ringbuffer that stores captured frames in
		            RGB8 or MONO8 format.
		*/
		StoreFrameBuffer *RB;

		//! \var bool CamSetup
		//! \brief A flag that indicated if the camera has been setup or not.
		bool CamSetup;

		/*! 	\var bool CamSetup
				\brief A flag that indicated if the camera has been initialized
		              or not.
		*/
		bool CamInit;

		/*! \var bool CaptureFlag
		    \brief A flag that is used to control when the capture loop in the
		            Capture_Trehad.
		*/            
		bool CaptureFlag;

		//! \var pthread_mutex_t dev1394_mutex
		//! \brief a mutex to control acces to dev1394.
		pthread_mutex_t dev1394_mutex;

		//! \var pthread_mutex_t CamReg_mutex
		//! \brief a mute to control the CameraRegister.
		pthread_mutex_t CamReg_mutex;

		/*! \var pthread_t capture_thread
			 \brief An identifier of the thread that runs the Capture_thread
			        function.
		*/	        
		pthread_t capture_thread;

		void cleanup();
		void cleanup_all();
		void cleanup_and_exit ();
		void Init_Camera();
		void Init_Camera(uint32_t n);
		void  SigIntHandler (int n);
		static void SigHandler_trampoline(int sig)
		{
			std::vector <Camera1394 *>::iterator cam;
			cam = CameraRegister.begin();
			(*cam)->SigIntHandler(sig);
		}
		friend void *Capture_Thread (void *arg);

	public:
		Camera1394 (uint32_t n) { Init_Camera (n); }
		Camera1394 () { Init_Camera (0); }
		~Camera1394 ();
		int Setup(uint32_t rbs, dc1394video_mode_t vMode, dc1394speed_t isoSpeed, dc1394framerate_t fRate, int n = 4, iFormat frmt = FRM_NONE);
		void Set_CaptureInterval();
		void Set_CaptureInterval(float fact);
		int Set_VideoMode (dc1394video_mode_t vMode)
		{
			dc1394error_t err;

			err=dc1394_video_set_mode(camera, vMode);
			DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not set video mode\n");
			return 0;
		}
		int Set_FrameRate (dc1394framerate_t fRate)
		{
			dc1394error_t err;

			err=dc1394_video_set_framerate(camera, fRate);
			DC1394_ERR_CLN_RTN(err,cleanup_and_exit(),"Could not set framerate\n");
			Set_CaptureInterval(0.75);
			return 0;
		}
		int Start_Transmission();
		int Stop_Transmission();
		int Process_Frame();
		int Get_Capture_Interval ()
		{
			return Capture_Interval;
		}
		void Print_Camera_Info()
		{
			if (!CamInit)
				return;
			dc1394_camera_print_info(camera, stdout);
		}
		void Print_Cameras_List();
		void Set_Filename_Base(const char *s);
		void Set_MaxFileCounter (uint32_t n);
		void Set_FileSaveType (FileSaveType fs);
		int Process_Next_Stored_Frame();
		void FlushStorageBuffer();
		void Start_Capture();
		void Finish_Capture();
		float Get_Video_FrameRate();
		void Get_Frame_Size (uint32_t &Rows, uint32_t &Cols);


		uFrame *Get_uFrame_Pointer (uint32_t idx)
		{
			return RB->Get_uFrame_Pointer(idx);
		}

		void Release_uFrame_Pointer (uint32_t idx)
		{
			RB->Release_uFrame_Pointer (idx);
		}

		uint32_t Get_Last_Captured_Frame_Index()
		{
			return RB->Get_Last_Captured_Frame_Index();
		}
};


void *Capture_Thread (void *arg);

#endif
