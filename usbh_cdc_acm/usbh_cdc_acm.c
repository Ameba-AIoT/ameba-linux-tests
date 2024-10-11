// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek USB test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>


int main( int argc, char *argv[] )
{
	int fd,nByte;
	int ret;
	char *uart0 = "/dev/ttyACM0";
	char send_buffer[512];
	char read_buffer[512];
	struct termios newtio, oldtio;
	int cycle_num;

	fd = open(uart0, O_RDWR | O_NOCTTY);
	if(fd < 0) {
		printf("open %s is failed: %d\n", uart0, ret);
		return ret;
	}
	
	ret = tcgetattr(fd, &oldtio);
	if(ret) {
		printf("SetupSerial 1 fail: %d\n", ret);
		return ret;
	}

	memset(&newtio, 0, sizeof(newtio));

	/*local receive*/
	newtio.c_cflag |= (CLOCAL | CREAD);

	/*8bit data*/
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag |= CS8;

	/*no parity and ignore parity error*/
	newtio.c_cflag &= ~PARENB;
	newtio.c_iflag = IGNPAR;

	/*set baudrate*/
	newtio.c_cflag |=  B9600;
	//cfsetispeed(&newtio, B9600);
	//cfsetospeed(&newtio, B9600);

	/*1 stop bit*/
	newtio.c_cflag &=  ~CSTOPB;

	/*time out*/
	newtio.c_cc[VTIME]	= 0;
	newtio.c_cc[VMIN] = 0;

	/*flush intput queue*/
	tcflush(fd, TCIFLUSH);

	/*enable ternios setting*/
	ret = tcsetattr(fd, TCSANOW, &newtio);
	if(ret)	{
		printf("COM set error\n");
		return -1;
	}

	printf("Make sure usb device is loopback mode. Start loopback now...\n");

	while(1) {
	
		memset(send_buffer, 0, 512);
		sprintf(send_buffer, "loopback cycle %d\n", cycle_num++);
		printf("send string: %s", send_buffer);
	
		ret = write(fd, send_buffer, strlen(send_buffer));

		while(1){
			if((nByte = read(fd, read_buffer, 512)) > 0) {
				printf("recv string: %s", read_buffer);
				memset(read_buffer, 0, 512);
				break;
			}
		}
	}
}




