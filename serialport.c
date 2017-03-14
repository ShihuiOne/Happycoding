#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h> /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdio.h>

#include "serialport.h"

/**********************************
*Name：OpenDevice
*Function：Open a serialport device
*Param：port: The device name which you are going to open
	ex:"/dev/ttyS1"
*Output：Success: fd for this device; fail: false
*********************************/

int OpenDevice(const char* port)
{
	
	int fd = open(port,O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == false)
	{
		printf("cannot open port: %s", port);
		return false;
	}
	//if the port is block
	
	if(fcntl(fd,F_SETFL,0) < 0)
	{
		printf("fcntl failed!\n");
		return false;
	}
	else 
	{
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
	}

	printf("fd->open=%d\n",fd );

	return fd;
}

/*******************************************************************
* Name： 	SetDevice
*Fuction ：  	Set up Device
*@Param：  fd 	
*           speed 		
*           flow_ctrl   
*           databits  7 or 8 
*           stopbits 1 or 2 
*           parity    N,E,O,,S                    
* Output：   Success:    true Fail:false
*******************************************************************/

int SetDevice(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	int   i;
    int   status;

    struct termios options;
    /*tcgetattr(fd,&options)
	get info
  	and save it in options,
    */
    if (tcgetattr(fd, &options) != 0)
    {
    	perror("SetupSerial 1");
    	return false;
    }

     //set Bound Rate for both input and output

    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
    	if  (speed == name_arr[i])
        {       
            cfsetispeed(&options, speed_arr[i]); 
            cfsetospeed(&options, speed_arr[i]);  
        }
    }
    //change control mode
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;

    //set folw control
    switch(flow_ctrl)
    {
      
       case 0 ://none
              options.c_cflag &= ~CRTSCTS;
              break;   
      
       case 1 ://hardware flow control
              options.c_cflag |= CRTSCTS;
              break;
       case 2 :// software flow control
              options.c_cflag |= IXON | IXOFF | IXANY;
              break;
    }

    //set data bits
    options.c_cflag &= ~CSIZE; //ignore the orther tag
    switch (databits)
    {  
       case 5 :
       			options.c_cflag |= CS5;
                break;
       case 6 :
                options.c_cflag |= CS6;
                break;
       case 7 :   
                options.c_cflag |= CS7;
                break;
       case 8 :    
                options.c_cflag |= CS8;
                break;  
       default:   
                printf("Unsupported data size\n");
                return false; 
    }
    //set parity bit
    switch (parity)
    {  
       case 'n':
       case 'N': //none
                options.c_cflag &= ~PARENB; 
                options.c_iflag &= ~INPCK;    
                break; 
       case 'o':  
       case 'O'://odd
                options.c_cflag |= (PARODD | PARENB); 
                options.c_iflag |= INPCK;             
                break; 
       case 'e': 
       case 'E'://even
                options.c_cflag |= PARENB;       
                options.c_cflag &= ~PARODD;       
                options.c_iflag |= INPCK;       
                break;
       case 's':
       case 'S': //space
                options.c_cflag &= ~PARENB;
                options.c_cflag &= ~CSTOPB;
                break; 
        default:  
                printf("Unsupported parity\n");   
                return false; 
    } 
    //set stop bits 
    switch (stopbits)
    {  
       case 1:   
                options.c_cflag &= ~CSTOPB; 
                break; 
       case 2:   
                options.c_cflag |= CSTOPB; 
                         break;
       default:   
                printf("Unsupported stop bits\n"); 
                return false;
    }
   
    //verify ouput mode ,raw data
    options.c_oflag &= ~OPOST;
   
    //set waiting time
    options.c_cc[VTIME] = 1; /* minimum wait time 1*(1/10)s */  
    options.c_cc[VMIN] = 20; /*minimum char number 1 */
   
    //if flow occurs
    tcflush(fd,TCIFLUSH);
   
    //activation
    if (tcsetattr(fd,TCSANOW,&options) != 0)  
    {
       perror("com set error!\n");  
       return false; 
    }

    return true; 
}

/*******************************************************************
* Name： 		InitDevice()
*Fuction ：  Init serialport	
*@Param：  fd 	
*           speed 		
*           flow_ctrl   
*           databits  7 or 8 
*           stopbits 1 or 2 
*           parity    N,E,O,,S                    
* Output：   Success:    true Fail:false
*******************************************************************/
int InitDevice(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	if(SetDevice(fd, speed, flow_ctrl, databits, stopbits, parity) == false)
	{
		return false;
	}
	else return true;
}

/*******************************************************************
* Name：                 Receive
*Fuction：              Receive data from serial port  	
*@Param：        fd 	
*                 rcv_buf   receiv buf
*                 data_len   length of a frame 
*Output ：      success:1,fail:0
*******************************************************************/
int Receive(int fd, char *rcv_buf,int data_len)
{
    int len,fs_sel;
    fd_set fs_read;
   
    struct timeval time;
   
    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);
   
    time.tv_sec = 10;
    time.tv_usec = 0;
    len = read(fd,rcv_buf, data_len);
    return len;
   
    //use select
    /*
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)
    {
    	len = read(fd,rcv_buf, data_len);
    	return len;
    }
    else
    {
    	return false;
    }  */
}

/*******************************************************************
* Name：               Send
*Fuction：              send data
*@Param：        fd      
*		 send_buf   
*              	 data_len    
*Output ：       Success  1 Fail 0
*******************************************************************/
int Send(int fd, char *send_buf,int data_len)
{
    int len = 0;
   
    len = write(fd,send_buf,data_len);
    if (len == data_len )
    {
    	return len;
    }     
    else   
    {
    	tcflush(fd,TCOFLUSH);
    	return false;
    }
}

/******************************************************
* Name：                Close_Dev
*Fuction：              close serial port
* @Param：        fd   
*Output：        void
*******************************************************************/
 
void Close_Dev(int fd)
{
    close(fd);
}
