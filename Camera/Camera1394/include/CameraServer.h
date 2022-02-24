/*  
	CameraServer.h - header file that defines the class CameraServer
	which creates a server that capture images  from a IEEE-1394 camera
	and send them to a client.
  
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

#ifndef __CAMERASERVER__
#define __CAMERASERVER__
#include <Camera1394.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <cstring>
#include <string>

#define BLOCKSIZE 32768

#define NumMsg 2


struct ImgHeader
{
   u_long r, c, l, RSize;
   long id;
};

class CameraServer;

struct HostInfo
{
	std::string inetaddr;
	int ClientFd;
	struct sockaddr_in SocketAddress;
	socklen_t SocketLength;
	void Init()
	{
		ClientFd = -1;
		memset(&SocketAddress, 0, sizeof(SocketAddress));
		SocketLength = sizeof(SocketAddress);
	}
	HostInfo(CameraServer *camser)
	{
		Init();
	}
};


class CameraServer: public Camera1394
{
	bool WFlag;
	int SocketFd;
	struct sockaddr_in SocketAddress;
	socklen_t SocketLength;
	int BackLog;
	public:
	CameraServer (int ncam, int port, int bl);
	void StartServer();

};

int Recv (int fd, char *data, long Bsz, int Flags);
int Send (int fd, char *data, long Bsz, int Flags);

int Send_Image (int Fd, void *Header, int HeadLength, u_char *data);
int Recv_Image (int Fd, void *Header, int HeadLength, u_char **data);
int Send_Image_doZ (int Fd, void *Header, int HeadLength, u_char *data, int leve4);
int Recv_Image_unZ (int Fd, void *Header, int HeadLength, u_char **data);

#endif
