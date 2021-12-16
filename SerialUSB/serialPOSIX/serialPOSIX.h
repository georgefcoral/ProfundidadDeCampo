#ifndef __SERIALPOSIX__
#define __SERIALPOSIX__

#include <unistd.h>
#include <iostream>
#include <termios.h>

//! Serial port POSIX implementation.
/*!
 * serialPOSIX is an interface for controlling serial port communications.
 * This uses, where possible, POSIX functions to configure
 * and communitate through a serial port.
 * 
 * For now, this interface let you:
 *  - Open a serial port
 *  - Close a serial port
 *  - Send data
 *  - Receive data
 *  - Set operation parameters:
 *	- Speed
 *	- Character size
 *	- Parity
 *	- Stop bits
 *  - Query port signals:
 *	- Carrier detect (CD)
 *	- Data Set Ready (DSR)
 *	- Clear To Send (CTS)
 *  - Set port signals:
 *	- Data Terminal Ready (DTR)
 *	- Request To Send (RTS)
 *  - Query how much data is pending to be read
 *
 *  There are some configurations that can't be controlled yet:
 *	- Canonical mode. Always disabled.
 *	- Echoing. Always disabled.
 *	- Software control flow. Always disabled.
 *	- Hardware control flow. Always disabled.
 *
 * TODO:
 *  This is done in a C-style way. The class must be updated to use
 *  exceptions.
 */

/*!
\class serialPOSIX
\brief This class implement an interface for controlling serial port communications.  This uses, where possible, POSIX functions to configure and communicate through a serial port.
 This interface allows you to: open a serial port, close a serial port, send data, receive data and set operation parameters: speed, character size, parity, stop bits. It also provides methods that allow you to query and set port signals: Carrier detect (CD), Data Set Ready (DSR), Clear To Send (CTS), Data Terminal Ready (DTR), Request To Send (RTS), and query how much data is pending to be read.
*/
class serialPOSIX
{
    protected:
	/*!
	\var int serialFD
	\brief Serial port file descriptor.
	*/
	int serialFD;

	/*!
	\var int serialError
	\brief The error code (copy of errno) set in case a failure is detected.
	*/
	int serialError;


	/*!
	\var char *serialFS
	\brief Serial port's filesystem device (e.g. /dev/ttyS0).
	*/
	char *serialFS; 

	/*!
	\var char *buffLastRead;
	\brief Pointer to last read data.
	*/
	char*buffLastRead; 

	/*!
	\var ssize_t bytesLastRead
	\brief Number of bytes of last read.
	*/
	ssize_t bytesLastRead;

	/*!
	\var ssize_t bytesReadBuffer
	\brief Size of buffLastRead variable.
	*/
	ssize_t bytesReadBuffer;
      
         /*Used for terminal baud rate*/
         speed_t baudRate;

    public:

    /*!
    \fn serialPOSIX(int rate)
    \brief Class constructor; this just set atributes for properly class initialization.
    */
	serialPOSIX(int rate=9600);

    /*!
    \fn serialPOSIX(const char *_serialFS,int rate)
    \brief Class constructor; Set initial values of atributes and tries to open the especified serial port. It is responsability of the user to check that the serial port was successfully open.
    \param _serialFS A pointer to a char string that contins the name of the serial port's filesystem device.
    */
	serialPOSIX(const char *_serialFS,int rate=9600);

    /*!
    \fn ~serialPOSIX()
    \brief Class destructor. It closes the serial port, if needed.  The destructor way to close a serial port should not be used. It is recommended that you manually close the serial port and check that it was sucessfully closed.
    */
	~serialPOSIX();

    /*!
    \fn bool openSerial(const char *_serialFS)
    \brief Try to open a serial port. If successfully, the serial port is configured as follow: - No parity, - 8 bits character size, 1 stop bits, 9600 bps.
    \param _serialFS a string containing the filesystem device.
    \return True if successfully
    */
	bool openSerial(const char *_serialFS);

    /*!
    \fn bool closeSerial()
    \brief Closes the serial port
    \return True if successfully or if there wasn't a port open.
    */
	bool closeSerial();

    /*!
    \fn ssize_t send(const char *_data, size_t _bytes)
    \brief Sends data through the serial port.
    \param _data  Pointer to the begining of the data to send.
    \param  _bytes Number of bytes to send.
    \return Numbers of bytes sent or -1 if error.
    */
	ssize_t send(const char *_data, size_t _bytes);

    /*!
    \fn ssize_t send(const char *_data)
    \brief Sends a string through the serial port.
    \param _data Pointer to a NULL terminated string.
    \return Numbers of bytes sent or -1 if error.
    */
	ssize_t send(const char *_data);

    /*!
    \fn char *receive(size_t _nbytes, ssize_t *_read)
    \brief Tries to read nbytes of data from the serial port.
    \param _nbytes   Max numbers of bytes to read
    \param _readed   Must be a valid pointer or NULL. Here is stored the numbers of bytes read. If -1, then there was an I/O error. If -2, can't allocate memory.
    \return Pointer to the received data or NULL if no data or error.This pointer must not be freed or deleted.
    */
	char *receive(size_t _nbytes, ssize_t *_read);

    /*!
    \fn char *receive(ssize_t *_bytes)
    \brief Tries to read all available data from the serial port.
    \param _bytes    Must be a valid pointer or NULL. Here is stored the numbers of bytes read. If -1, then  there was an error.
    \return Pointer to the received data or NULL if no data or error. This pointer must not be freed or deleted.
    */
	char *receive(ssize_t *_bytes);

    /*!
    \fn char *lastRead(ssize_t *_bytes)
    \brief Returns data of the last read.
    \param _bytes    Must be a valid pointer. Here is stored the numbers of bytes read. If -1, then there was an error.
    \return  Pointer to the received data or NULL if no data or error. This pointer must not be freed or deleted.
    */
	char *lastRead(ssize_t *_bytes);

    /*!
    \fn bool setInputOutputBaud(char _direction, speed_t _speed, int _actions = TCSANOW)
    \brief Sets the input and/or output speed of the serial port.
    \param _direction	If 'I', input speed is changed.	If 'O', output speed is changed. If 'B', input and output speed is changed.
    \param _speed	Speed constant as defined in termios.h. Consult cfsetospeed() man page for possible values.
    \param _actions	Optional actions. Consult tcsetattr() man page for possible values.
    \return  True if sucesfully.
    */
	bool setInputOutputBaud(char _direction, speed_t _speed, int _actions = TCSANOW);

    /*!
    \fn speed_t getInputOutputBaud(char _direction)
    \brief Gets the input or output speed of the serial port.
    \param _direction	If 'I', get input speed. If 'O', get output speed.
    \return  The speed constant as defined in termios.h. Consult cfsetospeed() man page for possible values. On failure (rare), this returns 0xFFFFFFFF.
    */
	speed_t getInputOutputBaud(char _direction);

    /*!
    \fn bool setCharSize(int _csize, int _actions = TCSANOW)
    \brief Sets character size.
    \param _csize    Valid values are CS{5|6|7|8}. Consult termios structure man page for more information.
    \param _actions	Optional actions. Consult tcsetattr() man page for possible values.
    \return  True if sucesfully.
    */
	bool setCharSize(int _csize, int _actions = TCSANOW);

    /*!
    \fn int getCharSize()
    \brief Gets current character size.
    \return  The character size constant. Valid values are CS{5|6|7|8}. Consult termios structure man page for more information. On failure (rare), this returns 0xFFFFFFFF.
    */
	int getCharSize();

    /*!
    \fn bool setParity(bool _even, bool _enable = true, int _actions = TCSANOW)
    \brief Sets data parity and/or enables/disables parity checking.
    \param _even	    If true, even parity, else, odd.
    \param _enable   If true, parity is enabled, else, is disabled.
    \param _actions	Optional actions. Consult tcsetattr() man page for possible values.
    \return  True if sucesfully.
    */
	bool setParity(bool _even, bool _enable = true, int _actions = TCSANOW);

    /*!
    \fn int getParity()
    \brief Gets the current data parity.
    \return  0 if no parity, 1 if even parity, 2 if odd parity, -1 if error.
    */
	int getParity();

    /*!
    \fn bool setStopBits(int _bits, int _actions = TCSANOW)
    \brief Sets number of stop bits.
    \param _bits	    Valid values are 1 or 2, for one or two stop bits respectively.
    \param _actions	Optional actions. Consult tcsetattr() man page for possible values.
    \return  True if sucesfully.
    */
	bool setStopBits(int _bits, int _actions = TCSANOW);

    /*!
    \fn int getStopBits()
    \brief Gets current number of stop bits.
    \return 1 if one stop bit, 2 if two stop bits, -1 if error.
    */
	int getStopBits();

    /*!
    \fn bool setOptions(int _options, int _actions = TCSANOW)
    \brief Sets the I/O speed, character size, parity and stop bits.
    \param _options  Mask for termios structure that sets speed, character size, parity and stop bits options. Consult termios man page for more information. For example, to set 9600bps, 8char size, no parity and 2 stop bits this should be: B9600|CS8|CSTOP
    \param _actions	Optional actions. Consult tcsetattr() man page for possible values.
    \return  True if sucesfully.
    */
	bool setOptions(int _options, int _actions = TCSANOW);

    /*!
    \fn int carrier()
    \brief Checks if DCD signal is received/set. Some devices doesn't use this signal and may appear always set/unset
    \return 1 if Carrier signal is set, 0 if not, -1 if error checking status.
    */
	int carrier();

    /*!
    \fn int deviceConnected()
    \brief Checks if DSR signal is received/set. Usually this means that a device is connected to the serial port.
    \returns 1 if DSR is set, 0 if not, -1 if error checking status
    */
	int deviceConnected();

    /*!
    \fn int canSend()
    \brief Checks if CTS signal is received/set. This means that device in the other side is ready to receive data.
    \returns 1 if device in the other side is ready, 0 if not, -1 if error checking status.
    */
	int canSend();

    /*!
    \fn bool becomeOnLine(bool _online)
    \brief Set/Clears the DTR signal.  Usually this means that "you" are "connected".
    \returns True if sucessfull.
    */
	bool becomeOnLine(bool _online);

    /*!
    \fn bool acceptData(bool _accept)
    \brief Set/Clears the RTS signal. Usually, this means that you are ready to send/receive data.
    \return True if sucessfully
    */
	bool acceptData(bool _accept);

    /*!
    \fn int bytesInBuffer()
    \brief Returns number of bytes received and pending to be read.
    \return Number of bytes on serial port input buffer. If -1, there was an error retreiving information.
    */
	int bytesInBuffer();

    /*!
    \fn bool isOpen()
    \brief Checks whether a serial port is open.
    \return True if a serial port is open.
    */
	bool isOpen();

    /*!
    \fn int errorNumber()
    \brief Returns the error code of the last error. This is really a copy of errno variable.  This is not updated when an operation is successful. This means this function may return not zero even if you call it after a sucessful operation.
    \return Error code of the last error.
    */
	int errorNumber();

    /*!
    \fn int getFD()
    \brief Returns the serial port file descritor.
    \return The serial port file descriptor.
    */
    int getFD()
    {
        return serialFD;
    }

};


/*!
\fn spped_t getTermiosSpeed (unsigned int speed, int convDirection)
\brief This function converts a transmition/reception speed from the real values to the values defined in termios.h and viceversa.
\param speed The speed to be converted.
\param convDirection The direction of the conversion; from real numbers to termios values (default behavoir), or fomr termios values to real values.
\return The real speed values (if convDirection==0), or the termios value (is conDirection!=0).
*/
speed_t getTermiosSpeed(unsigned int speed, int convDirection=0);


#endif
