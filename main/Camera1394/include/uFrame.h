/*  
  	uFrame.h - header file that defines a structure to store an image
	           captured from a camera.
  
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

#ifndef __UFRAME__
#define __UFRAME__

#include <dc1394/dc1394.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <time.h>

enum iFormat {FRM_NONE = 0, RGB8, MONO8};

struct uFrame
{
		uint32_t Cols, Rows, Size, Depth, Index, Frame_Index;
		iFormat format;
		uint64_t timestamp;
		uint8_t *image;
		pthread_mutex_t Frame_mutex;

		void Set_Timestamp()
		{
			time_t tm = time(0);

			timestamp = tm * 1000000;
		}
		uFrame ()
		{
			pthread_mutex_init(&Frame_mutex, NULL);
			pthread_mutex_lock (&Frame_mutex);
			Rows = Cols = Size = Depth = Index = Frame_Index = 0;
			image = 0;
			format = FRM_NONE;
			Set_Timestamp();
			pthread_mutex_unlock (&Frame_mutex);
		}

		uFrame (uint32_t R, uint32_t C, iFormat f=MONO8)
		{
			pthread_mutex_init(&Frame_mutex, NULL);
			pthread_mutex_lock (&Frame_mutex);
			format = f;
			Cols = C;
			Rows = R;
			Depth = format == MONO8 ? 1 : 3;
			Size = Rows * Cols * Depth;
			Index = Frame_Index = 0;
			if (Size)
				image = new uint8_t [Size];
			else
				image = 0;
			Set_Timestamp();
			pthread_mutex_unlock (&Frame_mutex);
		}

		uFrame (const uFrame &uF)
		{
			pthread_mutex_lock (&Frame_mutex);
			Rows = uF.Rows;
			Cols = uF.Cols;
			Size = uF.Size;
			format = uF.format;
			Depth = uF.Depth;
			Index = uF.Index;
			Frame_Index = uF.Frame_Index;
			if (Size)
			{
				image = new uint8_t[Size];
				memcpy (image, uF.image, Size * sizeof (uint8_t));
			}
			else
				image = 0;
			Set_Timestamp();
			pthread_mutex_unlock (&Frame_mutex);
		}
		
		~uFrame ()
		{
			pthread_mutex_lock (&Frame_mutex);
			if (image)
				delete[] image;
			pthread_mutex_unlock (&Frame_mutex);
			pthread_mutex_destroy (&Frame_mutex);
		}

		uFrame & operator = (const uFrame &uF)
		{
			pthread_mutex_lock (&Frame_mutex);
			Cols = uF.Cols;
			Rows = uF.Rows;
			Depth = uF.Depth;
			format = uF.format;
			Index = uF.Index;
			Frame_Index = uF.Frame_Index;
			timestamp = uF.timestamp;
			if (Size)
			{
				if (uF.Size != Size)
				{
					delete[] image;
					image = new uint8_t[uF.Size];
				}
			}
			else
				image = new uint8_t[uF.Size];

			Size = uF.Size;
			
			memcpy (image, uF.image, Size * sizeof (uint8_t));
			
			pthread_mutex_unlock (&Frame_mutex);
			return *this;
		}

		uFrame & operator = (dc1394video_frame_t *vf )
		{
			uint32_t S;
			dc1394bool_t is_color;

			pthread_mutex_lock (&Frame_mutex);
			Cols=vf->size[0];
			Rows=vf->size[1];
			Index = 0;
			Frame_Index = 0;

			
			switch (format)
			{
				case MONO8:
					Depth = 1;
					break;
				case RGB8:
					Depth = 3;
					break;
				default:
					dc1394_is_color (vf->color_coding, &is_color);

					if (is_color)
					{
						Depth = 3;
						format = RGB8;
					}
					else
					{
						Depth = 1;
						format = MONO8;
					}
			}		

			timestamp = vf->timestamp;
		

			S = Rows * Cols * Depth;
			if (Size)
			{
				if (S != Size)
				{
					delete[] image;
					image = new uint8_t[S];
				}	
			}
			else
				image = new uint8_t[S];

			Size = S;
			if (format == MONO8)
					dc1394_convert_to_MONO8 (vf->image, image, vf->size[0], vf->size[1], vf->yuv_byte_order, vf->color_coding, vf->data_depth);
			else
					dc1394_convert_to_RGB8 (vf->image, image, vf->size[0], vf->size[1], vf->yuv_byte_order, vf->color_coding, vf->data_depth);
			
			pthread_mutex_unlock (&Frame_mutex);
			return *this;
		}

		void Set_uFrame (uint32_t C, uint32_t R, iFormat f=MONO8)
		{
			uint32_t tmS = C * R;

			pthread_mutex_lock (&Frame_mutex);
			if (Size && (tmS != Size && (C != Cols || R != Rows)))
				delete[] image;
			Cols = C;
			Rows = R;
			Depth = f == MONO8 ? 1 : 3;
			Size = tmS * Depth;
			format = f;
			Index = 0;
			Frame_Index = 0;
			if (Size)
				image = new uint8_t [Size];
			else
				image = 0;
			Set_Timestamp();
			pthread_mutex_unlock (&Frame_mutex);
		}

		void Save_Frame(const std::string &name)
		{
			std::fstream fo;

			pthread_mutex_lock (&Frame_mutex);
			fo.open(name.c_str(), std::ios::out);
			if (format == MONO8)
				fo << "P5" << std::endl;
			else	
				fo << "P6" << std::endl;
			fo << "#Timestamp: " << timestamp << std::endl;
			fo << "#Index: " << Index << std::endl;
			fo << "#Frame Index: " << Frame_Index << std::endl;
			fo << Cols << " " << Rows << "\n" << 255 << std::endl;
			fo.write((char *)image, Size);
			fo.close();
			pthread_mutex_unlock (&Frame_mutex);
		}
		
		uint64_t Get_Timestamp()
		{
			pthread_mutex_lock (&Frame_mutex);
			return timestamp;
			pthread_mutex_unlock (&Frame_mutex);
		}
	
		void Set_Index(uint32_t idx)
		{
			Index = idx;
		}
	
		void Set_Frame_Index(uint32_t idx)
		{
			Frame_Index = idx;
		}

		uint32_t Get_Index()
		{
			return Index;
		}

		uint32_t Get_Frame_Index()
		{
			return Frame_Index;
		}

		void Set_Format (iFormat frmt)
		{
			format = frmt;
		}

		void Lock_Frame()
		{
			pthread_mutex_lock (&Frame_mutex);
		}
		
		void Unlock_Frame()
		{
			pthread_mutex_unlock (&Frame_mutex);
		}
};

#endif
