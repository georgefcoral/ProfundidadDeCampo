#include <ios>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>

#include <serialPOSIX.h>

#include <iostream>
using namespace std;

serialPOSIX::serialPOSIX(int rate)
    : serialFD(-1), serialError(0), serialFS(NULL),
      buffLastRead(NULL), bytesLastRead(0), bytesReadBuffer(0)
{
    baudRate=getTermiosSpeed(rate,0);
}


serialPOSIX::serialPOSIX(const char *_serialFS,int rate)
    : serialFD(-1), serialError(0), serialFS(NULL),
      buffLastRead(NULL), bytesLastRead(0), bytesReadBuffer(0)
{
    baudRate=getTermiosSpeed(rate,0);
    openSerial(_serialFS);
}

serialPOSIX::~serialPOSIX()
{
    closeSerial();
}

bool serialPOSIX::openSerial(const char *_serialFS)
{
    struct termios options;

    if ( _serialFS == NULL || isOpen() ) return false;
    
    try
    {
	if ( (serialFD = open(_serialFS, O_RDWR | O_NOCTTY )) == -1 )
	    throw -1;
	
	if ( fcntl(serialFD, F_SETFL, 0) == -1 ) throw -1;
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	options.c_cflag &= ~(CBAUD | CSIZE | PARENB | CSTOPB);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= ~(IXOFF | IXON | IXANY);
        options.c_cflag |= (CLOCAL | CREAD | baudRate | CS8);

	
        if ( tcsetattr(serialFD, TCSANOW, &options) == -1 ) throw -1;

	serialFS = new char [strlen(_serialFS) + 1];
	strcpy(serialFS, _serialFS);
    }
    catch (...)
    {
	serialError = errno;
	if ( serialFD != -1 )
	    if ( close(serialFD) == 0 )
		serialFD = -1;
    }

    return (serialFD != -1);
}

bool serialPOSIX::closeSerial()
{
    if ( isOpen() )
    {
	if ( close(serialFD) == 0 )
	{
	    delete [] serialFS;
	    serialFS = NULL;
	    serialFD = -1;
	    serialError = 0;

	    if ( buffLastRead != NULL )
	    {
		delete [] buffLastRead;
		buffLastRead = NULL;
		bytesLastRead = bytesReadBuffer = 0;
	    }
	}
	else
	    serialError = errno;
    }

    return (serialFD == -1);
}

ssize_t serialPOSIX::send(const char *_data, size_t _bytes)
{
    ssize_t n;

    if ( !isOpen() ) return -1;

    n = write(serialFD, _data, _bytes);
    if ( n == -1 ) serialError = errno;

    return n;
}

ssize_t serialPOSIX::send(const char *_data)
{
    return send(_data, strlen(_data));
}

char *serialPOSIX::receive(size_t _nbytes, ssize_t *_readed)
{
    char *retorno;

    if ( !isOpen() )
    {
	    cerr << "File " << serialFS << " not open." << endl;
	    if ( _readed != NULL ) *_readed = -1;
	        return NULL;
    }

    try
    {
	if ( buffLastRead != NULL )
	{
	    if ( bytesReadBuffer < signed(_nbytes) )
	    {
		delete [] buffLastRead;
		buffLastRead = new char [_nbytes + 1];
		bytesReadBuffer = _nbytes;
	    }
	}
	else
	{
	    buffLastRead = new char [_nbytes + 1];
	}

	bytesLastRead = read(serialFD, buffLastRead, _nbytes);
	if ( bytesLastRead > 0 )
	{
	    retorno = buffLastRead;
	    buffLastRead[bytesLastRead] = '\0';
	}
	else
	{
	    retorno = NULL;
	}
	if ( _readed != NULL ) *_readed = bytesLastRead;
	if ( bytesLastRead == -1 ) serialError = errno;
    }
    catch (...)
    {
	buffLastRead = retorno = NULL;
	bytesLastRead = bytesReadBuffer = 0;
	if ( _readed != NULL ) *_readed = -2;
    }
    
    return retorno;
}

char *serialPOSIX::receive(ssize_t *_bytes)
{
    return receive(bytesInBuffer(), _bytes);
}

char *serialPOSIX::lastRead(ssize_t *_bytes)
{
    *_bytes = bytesLastRead;
    return buffLastRead;
}

bool serialPOSIX::setInputOutputBaud(char _direction, speed_t _speed, int _actions)
{
    struct termios options;
    bool ret = true;

    if ( !isOpen() ) return false;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;

	if ( _direction == 'I' || _direction == 'B' )
	    if ( cfsetispeed(&options, _speed) == -1 ) throw -1;
	if ( _direction == 'O' || _direction == 'B' )
	    if ( cfsetospeed(&options, _speed) == -1 ) throw -1;

	if ( tcsetattr(serialFD, _actions, &options) == -1 ) throw -1;
    }
    catch (...)
    {
	ret = false;
	serialError = errno;
    }

    return ret;
}

speed_t serialPOSIX::getInputOutputBaud(char _direction)
{
    struct termios options;
    speed_t speed=0;

    if ( !isOpen() ) return 0xFFFFFFFF;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	if ( _direction == 'I' ) speed = cfgetispeed(&options);
	if ( _direction == 'O' ) speed = cfgetospeed(&options);
    }
    catch (...)
    {
	speed = 0xFFFFFFFF;
	serialError = errno;
    }

    return speed;
}

bool serialPOSIX::setCharSize(int _csize, int _actions)
{
    struct termios options;
    bool ret = true;

    if ( !isOpen() ) return false;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= _csize;
	if ( tcsetattr(serialFD, _actions, &options) == -1 ) throw -1;
    }
    catch (...)
    {
	ret = false;
	serialError = errno;
    }

    return ret;
}

int serialPOSIX::getCharSize(void)
{
    struct termios options;
    int ret;

    if ( !isOpen() ) return 0xFFFFFFFF;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	ret = options.c_cflag & CSIZE;
    }
    catch (...)
    {
	ret = 0xFFFFFFFF;
	serialError = errno;
    }

    return ret;
}

bool serialPOSIX::setParity(bool _even, bool _enable, int _actions)
{
    struct termios options;
    bool ret = true;

    if ( !isOpen() ) return false;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;

	if ( _enable )
	    options.c_cflag |= PARENB;
	else
	    options.c_cflag &= ~PARENB;

	if ( _even )
	    options.c_cflag &= ~PARODD;
	else
	    options.c_cflag |= PARODD;

	if ( tcsetattr(serialFD, _actions, &options) == -1 ) throw -1;
    }
    catch (...)
    {
	ret = false;
	serialError = errno;
    }

    return ret;
}

int serialPOSIX::getParity(void)
{
    struct termios options;
    int ret;

    if ( !isOpen() ) return -1;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	if ( options.c_cflag & PARENB )
	{
	    if ( options.c_cflag & PARODD )
		ret = 2;
	    else
		ret = 1;
	}
	else
	    ret = 0;
    }
    catch (...)
    {
	ret = -1;
	serialError = errno;
    }

    return ret;
}

bool serialPOSIX::setStopBits(int _bits, int _actions)
{
    struct termios options;
    bool ret = true;        sleep(2);

    if ( !isOpen() ) return false;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;

	if ( _bits == 1 )
	    options.c_cflag &= ~CSTOPB;
	else
	    options.c_cflag |= CSTOPB;

	if ( tcsetattr(serialFD, _actions, &options) == -1 ) throw -1;
    }
    catch (...)
    {
	ret = false;
	serialError = errno;
    }

    return ret;
}

int serialPOSIX::getStopBits(void)
{
    struct termios options;
    int ret;

    if ( !isOpen() ) return -1;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;
	if ( options.c_cflag & CSTOPB )
	    ret = 2;
	else
	    ret = 1;
    }
    catch (...)
    {
	ret = -1;
	serialError = errno;
    }

    return ret;
}

bool serialPOSIX::setOptions(int _options, int _actions)
{
    struct termios options;
    bool ret = true;

    if ( !isOpen() ) return false;

    try
    {
	if ( tcgetattr(serialFD, &options) == -1 ) throw -1;

	options.c_cflag &= ~CBAUD;
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~PARODD;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= _options;

	if ( tcsetattr(serialFD, _actions, &options) == -1 ) throw -1;
    }
    catch (...)
    {
	ret = false;
	serialError = errno;
    }

    return ret;
}

int serialPOSIX::carrier(void)
{
    int status;

    if ( !isOpen() ) return -1;

    if ( ioctl(serialFD, TIOCMGET, &status) == -1 )
    {
	serialError = errno;
	return -1;
    }

    return ((status & TIOCM_CAR) != 0) ? 1:0;
}

int serialPOSIX::deviceConnected(void)
{
    int status;

    if ( !isOpen() ) return -1;

    if ( ioctl(serialFD, TIOCMGET, &status) == -1 )
    {
	serialError = errno;
	return -1;
    }

    return ((status & TIOCM_DSR) != 0) ? 1:0;
}

int serialPOSIX::canSend(void)
{
    int status;

    if ( !isOpen() ) return -1;

    if ( ioctl(serialFD, TIOCMGET, &status) == -1 )
    {
	serialError = errno;
	return -1;
    }

    return (status & TIOCM_CTS) ? 1:0;
}

bool serialPOSIX::becomeOnLine(bool _online)
{
    int status;

    if ( !isOpen() ) return false;

    try
    {
	if ( ioctl(serialFD, TIOCMGET, &status) == -1 ) throw -1;

	if ( _online )
	    status |= TIOCM_DTR;
	else
	    status &= ~TIOCM_DTR;

	if ( ioctl(serialFD, TIOCMSET, &status) == -1 ) throw -1;
    }
    catch (...)
    {
	serialError = errno;
	return false;
    }

    return true;
}

bool serialPOSIX::acceptData(bool _accept)
{
    int status;

    if ( !isOpen() ) return false;

    try
    {
	if ( ioctl(serialFD, TIOCMGET, &status) == -1 ) throw -1;

	if ( _accept )
	    status |= TIOCM_RTS;
	else
	    status &= ~TIOCM_RTS;

	if ( ioctl(serialFD, TIOCMSET, &status) == -1 ) throw -1;
    }
    catch (...)
    {
	serialError = errno;
	return false;
    }

    return true;
}

int serialPOSIX::bytesInBuffer(void)
{
    int bytes;

    if ( !isOpen() ) return -1;

    if ( ioctl(serialFD, FIONREAD, &bytes) == -1 )
    {
	serialError = errno;
	bytes = -1;
    }
    
    return bytes;
}

bool serialPOSIX::isOpen()
{
    return (serialFD != -1);
}

int serialPOSIX::errorNumber(void)
{
    return serialError;
}

speed_t getTermiosSpeed(unsigned int speed, int convDirection)
{

    if (!convDirection)
    {   
        speed_t val;

        switch (speed)
        {
            case 50: val = B50; 
                        break;
            case 75: val = B75; 
                        break;
            case 110: val = B110; 
                        break;
            case 134: val = B134; 
                        break;
            case 150: val = B150; 
                        break;
            case 200: val = B200; 
                        break;
            case 300: val = B300; 
                        break;
            case 600: val = B600; 
                        break;
            case 1200: val = B1200; 
                        break;
            case 1800: val = B1800; 
                        break;
            case 2400: val = B2400; 
                        break;
            case 4800: val = B4800; 
                        break;
            case 9600: val = B9600; 
                        break;
            case 19200: val = B19200; 
                        break;
            case 38400: val = B38400; 
                        break;
            case 57600: val = B57600; 
                        break;
            case 115200: val = B115200; 
                        break;
            case 230400: val = B230400; 
                        break;
            case 460800: val = B460800; 
                        break;
            case 500000: val = B500000; 
                        break;
            case 576000: val = B576000; 
                        break;
            case 921600: val = B921600; 
                        break;
            case 1000000: val = B1000000; 
                        break;
            case 1152000: val = B1152000; 
                        break;
            case 0: 
            default:
                val = B0; 
        };
        return val;
    }
    else
    {   
        int val;

        switch (speed)
        {
            case B50: val = 50;
                        break;
            case B75: val = 75;
                        break;
            case B110: val = 110;
                        break;
            case B134: val = 134;
                        break;
            case B150: val = 150;
                        break;
            case B200: val = 200;
                        break;
            case B300: val = 300;
                        break;
            case B600: val = 600;
                        break;
            case B1200: val = 1200;
                        break;
            case B1800: val = 1800;
                        break;
            case B2400: val = 2400;
                        break;
            case B4800: val = 4800;
                        break;
            case B9600: val = 9600;
                        break;
            case B19200: val = 19200;
                        break;
            case B38400: val = 38400;
                        break;
            case B57600: val = 57600;
                        break;
            case B115200: val = 115200;
                        break;
            case B230400: val = 230400;
                        break;
            case B460800: val = 460800;
                        break;
            case B500000: val = 500000;
                        break;
            case B576000: val = 576000;
                        break;
            case B921600: val = 921600;
                        break;
            case B1000000: val = 1000000;
                        break;
            case B1152000: val = 1152000;
                        break;
            case B0:
            default:
             val = 0; 

        };
    return val;
    }
}
