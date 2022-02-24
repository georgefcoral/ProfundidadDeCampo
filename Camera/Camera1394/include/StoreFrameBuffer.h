/*  
 
 	 StoreFrameBuffer.h - header file of an especialization of the Ringbuffer
	 class to store uFrames.

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

#ifndef __STOREFRAMEEBUFFER__
#define __STOREFRAMEBUFFER__

#include <RingBuffer.h>
#include <uFrame.h>
#include <dc1394/dc1394.h>
#include <string>

enum FileSaveType {FST_NONE, FST_ALL, FST_RINGBUFFER};

class StoreFrameBuffer: public RingBuffer <uFrame>
{
	std::string filename_base;
	uint32_t Counter, MaxCounter, Index, Last_Captured_Frame;
	FileSaveType FS;

	void Set_Name(std::string &name)
	{
		char n[1024];

		snprintf (n, 1023, "%s_%06d.pnm", filename_base.c_str(), Counter);
		name = n;
	}

	public:
		StoreFrameBuffer (uint32_t N, iFormat frmt = FRM_NONE): RingBuffer<uFrame>(N)
		{
			std::string s("FrameImage");

			pthread_mutex_lock(&RB_mutex);
			filename_base = s;
			Counter = 0;
			Index = 0;
			FS = FST_ALL;
			MaxCounter = 0;
			Last_Captured_Frame = 0;
			if (frmt != FRM_NONE)
				for (uint32_t i = 0 ; i < N ; ++i)
					R[i].Set_Format (frmt);
			pthread_mutex_unlock(&RB_mutex);
		}

		void Set_FileSaveType(FileSaveType Fss)
		{
			FS = Fss;
		}

		void Set_MaxCounter(uint32_t n)
		{
			MaxCounter = n;
		}	
		
		void Set_Filename_Base(const char *filename)
		{
			std::string s(filename);
			filename_base = s;
		}
		
		int Queue (dc1394video_frame_t *vf)
		{
			pthread_mutex_lock(&RB_mutex);
			if (T + 1  != H)
			{
				uFrame *Fr;
				uint32_t idx = (uint32_t)T;

				Fr = R + idx;

				*Fr = vf;
				Fr->Set_Index(Index);
				Fr->Set_Frame_Index(idx);
				Index++;
				Last_Captured_Frame = idx;
				T++;
				pthread_mutex_unlock(&RB_mutex);
				return 0;
			}
			else
			{
#ifdef VERBOSE
			std::cerr << "Se lleno el StoreFrameBuffer\n";
#endif			
				//Dequeue ()
				if (H != T)
				{
					std::string name;

					if (FS != FST_NONE)
					{
						Set_Name(name);
						R[(uint32_t)H].Save_Frame(name);
					}
					H++;
					if ( FS == FST_RINGBUFFER && Counter == MaxCounter)
						Counter = 0;
 					else
						Counter++;
				}

				if (T + 1  != H) //Queue(vf)
				{
					uFrame *Fr;
					uint32_t idx = (uint32_t)T;

					Fr = R + idx;

					*Fr = vf;
					Fr->Set_Index(Index);
					Fr->Set_Frame_Index(idx);
					Index++;
					Last_Captured_Frame = idx;
					T++;
					pthread_mutex_unlock(&RB_mutex);
					return -2;
				}
				else
				{
					return -1;
					pthread_mutex_lock(&RB_mutex);
				}
			}
		}
		int Dequeue()
		{
			pthread_mutex_lock(&RB_mutex);
			if (H != T)
			{
				std::string name;
				if (FS != FST_NONE)
				{
					Set_Name(name);
					R[(uint32_t)H].Save_Frame(name);
				}
				H++;
				if ( FS == FST_RINGBUFFER && Counter == MaxCounter)
					Counter = 0;
 				else
					Counter++;
				pthread_mutex_unlock(&RB_mutex);
				return 0;
			}
			pthread_mutex_unlock(&RB_mutex);
			return -1;
		}

		int Dequeue(uFrame &uf)
		{
			pthread_mutex_lock(&RB_mutex);
			if (H != T)
			{
				std::string name;
				uf = R[(uint32_t)H];
				if (FS != FST_NONE)
				{
					Set_Name(name);
					uf.Save_Frame(name);
				}
				H++;
				if ( FS == FST_RINGBUFFER && Counter == MaxCounter)
					Counter = 0;
 				else
					Counter++;
				pthread_mutex_unlock(&RB_mutex);
				return 0;
			}
			pthread_mutex_unlock(&RB_mutex);
			return -1;
		}

		void Flush()
		{
			while (!Dequeue());
		}

		void Lock_Frame(uint32_t idx)
		{
				R[T+idx].Lock_Frame();
		}

		void Unlock_Frame(uint32_t idx)
		{
				R[T+idx].Unlock_Frame();
		}

		uFrame *Get_uFrame_Pointer (uint32_t idx)
		{
			uFrame *uF;

			pthread_mutex_lock(&RB_mutex);
			R[idx].Lock_Frame();
			uF = &(R[idx]);
			pthread_mutex_unlock(&RB_mutex);
			return uF;
		}

		void Release_uFrame_Pointer (uint32_t idx)
		{
				R[idx].Unlock_Frame();
		}

		uint32_t Get_Last_Captured_Frame_Index()
		{
			return Last_Captured_Frame;
		}

};

#endif
