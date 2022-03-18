/*  
 	CameraServer.cpp - The implementation of the methods of the classa
	CameraServer which creates a server that capture images  from a 
	IEEE-1394 camera and send them to a client.
	
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
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <zlib.h>
#include <endian.h>

#define	Flip_int16(type)  (((type >> 8) & 0x00ff) | ((type << 8) & 0xff00))

#define	Flip_int32(type)  (((type >>24) & 0x000000ff) | ((type >> 8) & 0x0000ff00) | ((type << 8) & 0x00ff0000) | ((type <<24) & 0xff000000) )

#ifdef BYTE_ORDER
# if BYTE_ORDER == LITTLE_ENDIAN
#  define I_AM_LITTLE_ENDIAN
# else
#  if BYTE_ORDER == BIG_ENDIAN
#   define I_AM_BIG_ENDIAN
#  endif
# endif
#endif /* __BYTE_ORDER */


char ComMsg[2][4]={"BYE", "SND"};

CameraServer::CameraServer(int ncam, int port, int bl=5): Camera1394(ncam)
{
	SocketFd = 0;
	BackLog = bl;
	WFlag = false;
	try
	{
		if ((SocketFd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
			throw errno;
	}
	catch (int err)
	{
		char buff[512];

		strerror_r (err, buff, 511);
		std::cerr << "Socket Error: " << buff << std::endl;
		exit(1);
	}

	SocketAddress.sin_family = AF_INET;
   SocketAddress.sin_port = htons (port);
   SocketAddress.sin_addr.s_addr = htonl (INADDR_ANY);
	SocketLength = sizeof(SocketAddress);

	try
	{
		if (bind (SocketFd, (struct sockaddr *) &SocketAddress,
        SocketLength) == -1)
        throw errno;
   }     
	catch (int err)
	{
		char buff[512];

		strerror_r (err, buff, 511);
		std::cerr << "Bind Error: " << buff << std::endl;
		exit(1);
	}

	try
	{
		if (listen (SocketFd, BackLog) == -1)
			throw errno;
	}
	catch (int err)
	{
		char buff[512];

		strerror_r (err, buff, 511);
		std::cerr << "Listen Error: " << buff << std::endl;
		exit(1);
	}		
}

void CameraServer::StartServer ()
{
	bool SFlag;
	WFlag = 1;

	while (WFlag)
	{
		HostInfo Cli(this);

		try
		{
			if ( (Cli.ClientFd = accept (SocketFd, (struct sockaddr *)&(Cli.SocketAddress), (socklen_t *) &(Cli.SocketLength))) == -1)
          	throw errno;
		}
		catch (int err)
		{
			char buff[512];

			strerror_r (err, buff, 511);
			std::cerr << "Listen Error: " << buff << std::endl;
			exit(1);
		}
		Cli.inetaddr = inet_ntoa(Cli.SocketAddress.sin_addr);
		std::cout << "Atendiendo a: " << Cli.inetaddr << std::endl;

		SFlag = true;

		while (SFlag)
		{
			char msg[32];
			int sel;

			Recv (Cli.ClientFd, msg, 4, 0);
			for (sel=0; sel < NumMsg; sel++)
				if (!strncmp(msg, ComMsg[sel], strlen(ComMsg[sel])))
					break;
			switch (sel)
			{
				case 0: //BYE
					std::cout << "Recibimos BYE" << std::endl;
					std::cout.flush();
					SFlag = false;
					break;
				case 1: //SEND_IMAGE:
					std::cout << "Recibimos SEND_IMAGE" << std::endl;
					std::cout.flush();
					{
						 uint32_t idx, Rows, Cols;
						 uFrame *uF;
						 ImgHeader Head;

					 	do
					 	{
					 		idx = Get_Last_Captured_Frame_Index();
					 		Get_Frame_Size (Rows, Cols);
					 		uF = Get_uFrame_Pointer (idx);
							if (uF->Size)
								break;
							Release_uFrame_Pointer(idx);
						} while (true);
						Head.l = 1;
						Head.r = Rows;;
						Head.c = Cols;
						Head.RSize = uF->Size;
						Head.id = 1;
						Send_Image (Cli.ClientFd, &Head, sizeof(Head), uF->image);
						Release_uFrame_Pointer(idx);
					}
					break;
				default:
					std::cout << "Quien sabe que recibimos." << std::endl;
					std::cout.flush();
			}
		}
		close (Cli.ClientFd);
		Cli.Init();
	}
}

int Recv (int fd, char *data, long Bsz, int Flags)
{
	register char *apu = data;
	long tmp = Bsz, BytesTransferred;

	if (fd)
	{
		do
		{
			try
			{
				BytesTransferred = recv (fd, apu, Bsz, Flags);
				if (BytesTransferred == -1)
					throw errno;
			}
			catch(int err)
			{
				char buff[512];

				strerror_r (err, buff, 511);
				std::cerr << "Recv Error: " << buff << std::endl;
				return -1;
			}
			apu += BytesTransferred;
			Bsz -= BytesTransferred;
		} while (Bsz);
		return tmp;
	}
	return -1;
}


int Send (int fd, char *data, long Bsz, int Flags)
{
	register char *apu = data;
	long tmp = Bsz, BytesTransferred;

	if (fd)
	{
		do
		{
			try
			{
				BytesTransferred = send (fd, apu, Bsz, Flags);
				if (BytesTransferred == -1)
					throw errno;
			}		
			catch(int err)
			{
				char buff[512];

				strerror_r (err, buff, 511);
				std::cerr << "Send Error: " << buff << std::endl;
				return -1;
			}
			apu += BytesTransferred;
			Bsz -= BytesTransferred;
		}
		while (Bsz);
		return tmp;
	}
	return -1;
}

int Send_Image (int Fd, void *Header, int HeadLength, u_char *data)
{
	ImgHeader *h;
	u_long dest_len, bytes = 0;


	h = (ImgHeader *) Header;

	dest_len = h->RSize;

#ifdef I_AM_BIG_ENDIAN
	h->r = Flip_int32(h->r);
	h->c = Flip_int32(h->c);
	h->l= Flip_int32(h->l);
	h->RSize= Flip_int32(h->RSize);
	h->id= Flip_int32(h->id);
#endif

	bytes = Send (Fd, (char *) Header, HeadLength, 0);
	bytes += Send (Fd, (char *) data, dest_len, 0);

	return bytes;
}






int Recv_Image (int Fd, void *Header, int HeadLength, u_char **data)
{
	ImgHeader *h;
	u_long dest_len;
	u_long bytes;

	bytes = Recv (Fd, (char *) Header, HeadLength, 0);

	h = (ImgHeader *) Header;

#ifdef I_AM_BIG_ENDIAN
	h->r = Flip_int32(h->r);
	h->c = Flip_int32(h->c);
	h->l= Flip_int32(h->l);
	h->RSize= Flip_int32(h->RSize);
	h->id= Flip_int32(h->id);
#endif

	dest_len = h->RSize;

	*data = (u_char *) calloc (dest_len, sizeof (u_char));
	bytes += Recv (Fd, (char *) *data, dest_len, 0);

	return bytes;
}


int
Send_Image_doZ (int Fd, void *Header, int HeadLength, u_char *data, int level)
{
	ImgHeader *h;
	u_long Size, dest_len, bytes = 0;
	u_char *zbuff;
	int err;


	h = (ImgHeader *) Header;
	Size = h->l * h->r * h->c;

	dest_len = (u_long) (Size * 1.1) + 12;
	zbuff = (u_char *) calloc (dest_len, sizeof (u_char));
	err = compress2 (zbuff, &dest_len, data, Size, level);
	if (err != Z_OK)
	{
		fprintf (stderr, "Error Compressing in Send_zImage.\n");
		free (zbuff);
		return 0;
	}

	printf ("Image Z-Compressed from %ld to %ld\n", Size, dest_len);

	h->RSize = dest_len;

#ifdef I_AM_BIG_ENDIAN
	h->r = Flip_int32(h->r);
	h->c = Flip_int32(h->c);
	h->l= Flip_int32(h->l);
	h->RSize= Flip_int32(h->RSize);
	h->id= Flip_int32(h->id);
#endif

	bytes = Send (Fd, (char *) Header, HeadLength, 0);
	bytes += Send (Fd, (char *) zbuff, dest_len, 0);

	return bytes;
}

int Recv_Image_unZ (int Fd, void *Header, int HeadLength, u_char **data)
{
	ImgHeader *h;
	u_long dest_len, Size;
	u_char *zbuff;
	u_long bytes;
	int err;

	bytes = Recv (Fd, (char *) Header, HeadLength, 0);

	h = (ImgHeader *) Header;

#ifdef I_AM_BIG_ENDIAN
	h->r = Flip_int32(h->r);
	h->c = Flip_int32(h->c);
	h->l= Flip_int32(h->l);
	h->RSize= Flip_int32(h->RSize);
	h->id= Flip_int32(h->id);
#endif

	dest_len = h->RSize;
	Size = h->l * h->r * h->c;

	zbuff = (u_char *) calloc (dest_len, sizeof (u_char));
	*data = (u_char *) calloc (Size, sizeof (u_char));

	bytes += Recv (Fd, (char *) zbuff, dest_len, 0);

	err = uncompress (*data, &Size, zbuff, dest_len);
	if (err != Z_OK)
	{
		fprintf (stderr, "Error Uncompressing in Recv_zImage.\n");
		free (zbuff);
		free (*data);
		*data = 0;
		return 0;
	}

	h->RSize = Size;
	free (zbuff);

	return bytes;
}

