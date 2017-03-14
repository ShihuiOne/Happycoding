/*****************************************
author:SHihuiOne
Email:shihui.one@gmail.com
Date:2017-02-14
****************************************/
#ifndef SERIALPORT_H
#define SERIALPORT_H

 /****************************************************************
 * Constants
 ****************************************************************/
#define false 0
#define true 1

static int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
static int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};

int SetDevice(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
int OpenDevice(const char* port);
void Close_Dev(int fd);
int InitDevice(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
int Receive(int fd, char *rcv_buf,int data_len);
int Send(int fd, char *send_buf,int data_len);


#endif  
