// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek UART test
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

static void usage(const char *program) {
	fprintf(stderr, "Usage: %s [options]\n", program);
	fprintf(stderr, "\n");
	fprintf(stderr, "  -P 0/1/2  \n");
}

int main( int argc, char *argv[] )
{
	static const char optstr[] = "P:";
	int port = 2;
	int c, fd,nByte;
	int ret;
	char buffer[512];
	char uart[13] = "/dev/ttyRTK";
	char *uart_out = "loopback mode...\n";
	struct termios newtio, oldtio;

	while ((c = getopt(argc, argv, optstr)) != -1) {
		switch (c) {
			case 'P':
				port = atoi(optarg);
				fprintf(stderr, "port: %d\n", port);
				break;
			default:
				usage(argv[0]);
				return 0;
		}
	}

	sprintf(uart, "/dev/ttyRTK%d", port);
	fprintf(stderr, "uart: %s\n", uart);

	memset(buffer, 0, sizeof(buffer));

	fd = open(uart, O_RDWR | O_NOCTTY);
	if(fd < 0) {
		printf("Fail to open device node %s: %d\n", uart, ret);
		return ret;
	}

	ret = tcgetattr(fd, &oldtio);
	if(ret) {
		printf("Fail to get attribute: %d\n", ret);
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
		printf("Fail to set attribute: %d\n", ret);
		return -1;
	}

	ret = write(fd, uart_out, strlen(uart_out));
	if(ret != strlen(uart_out)) {
		printf("Fail to write: %d", ret);
		return ret;
	}

	printf("Start loopback...\n");

	while(1) {
		while((nByte = read(fd, buffer, 512))>0) {
			buffer[nByte+1] = '\0';
			ret = write(fd,buffer,strlen(buffer));
			if(ret != strlen(buffer)) {
				printf("Fail to write: %d", ret);
				return ret;
			}

			memset(buffer, 0, strlen(buffer));
			nByte = 0;
		}
	}
}
