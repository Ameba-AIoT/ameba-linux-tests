// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek USB test
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <stdbool.h>
#include <sys/param.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define F_WIDTH		1024
#define F_HEIGHT	768
#define F_FORMAT	V4L2_PIX_FMT_MJPEG
#define F_FPS		15

int image_height;
int image_width;
unsigned char *image_buffer[4];
int uvc_video_fd;


int video_device_init(char *DEVICE_NAME)
{
	struct v4l2_format format;
	struct v4l2_fmtdesc fmt;
	struct v4l2_buffer buff_info;
	unsigned int fps;
	int i;
	int Type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct v4l2_streamparm streamparm;
	struct v4l2_requestbuffers req_buff;

	uvc_video_fd = open(DEVICE_NAME, O_RDWR);
	if(uvc_video_fd < 0) {
		printf("%s Fail to open video device\n", DEVICE_NAME);
		return -1;
	}else{
		printf("Open video device success\n");
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.index = 0;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	while(ioctl(uvc_video_fd, VIDIOC_ENUM_FMT, &fmt) == 0){
		fmt.index ++ ;
		printf("<'%c%c%c%c'--'%s'>\n", 
			   fmt.pixelformat & 0xff, (fmt.pixelformat >> 8) & 0xff,
			   (fmt.pixelformat >> 16) & 0xff,(fmt.pixelformat >> 24) & 0xff,
			   fmt.description);
	}

	memset(&format, 0, sizeof(struct v4l2_format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = F_WIDTH;
	format.fmt.pix.height = F_HEIGHT;
	format.fmt.pix.pixelformat = F_FORMAT;
	format.fmt.pix.field = V4L2_FIELD_ANY;
	
	if(ioctl(uvc_video_fd, VIDIOC_S_FMT, &format)){
		printf("Fail to set format\n");
		return -2;
	}
	
	image_width = format.fmt.pix.width;
	image_height = format.fmt.pix.height;

	if(format.fmt.pix.pixelformat == F_FORMAT) {
		printf("Set format to JEPG!\n");
	} else {
		printf("Format not support\n");
		return -3;
	}

	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	streamparm.parm.capture.timeperframe.numerator = 1;
	streamparm.parm.capture.timeperframe.denominator = F_FPS;

	printf("Set parameter: %d*%d@%dfps\n", F_WIDTH, F_HEIGHT, F_FPS);

	if(ioctl(uvc_video_fd, VIDIOC_S_PARM, &streamparm)) {
		printf("Fail to set FPS\n");
		return -3;
	}

	if(ioctl(uvc_video_fd, VIDIOC_S_PARM, &streamparm)){
		printf("Fail to set FPS\n");
		return -3;
	}

	printf("Actual parameter: %d*%d@%dfps\n", image_width, image_height, 
				streamparm.parm.capture.timeperframe.denominator);

	memset(&req_buff, 0, sizeof(struct v4l2_requestbuffers));
	req_buff.count = 4;
	req_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req_buff.memory = V4L2_MEMORY_MMAP;
	
	if(ioctl(uvc_video_fd, VIDIOC_REQBUFS, &req_buff)){
		printf("Fail to alloc video buffer!\n");
		return -4;
	}
	
	printf("Video buffer number: %d\n", req_buff.count);
	memset(&buff_info, 0, sizeof(struct v4l2_buffer));

	for(i = 0; i < req_buff.count; i++){
		buff_info.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buff_info.memory=V4L2_MEMORY_MMAP;
	
		if(ioctl(uvc_video_fd, VIDIOC_QUERYBUF, &buff_info)){
			printf("Fail to get information of video buffer!\n");
			return -5;
		}

		image_buffer[i] = mmap(NULL, buff_info.length, PROT_READ|PROT_WRITE,
								MAP_SHARED, uvc_video_fd, buff_info.m.offset);
		if(image_buffer[i] == NULL){
			printf("Fail to mmap video buffer!\n");
			return -6;
		}
	}

	memset(&buff_info, 0, sizeof(struct v4l2_buffer));
	
	for(i = 0; i < req_buff.count; i++){
		buff_info.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buff_info.index = i;
		buff_info.memory = V4L2_MEMORY_MMAP;
		if(ioctl(uvc_video_fd, VIDIOC_QBUF, &buff_info)){
			printf("Fail to queue buffer!\n");
			return -7;
		}
	}

	if(ioctl(uvc_video_fd, VIDIOC_STREAMON, &Type)){
		printf("Fail to start capture\n");
		return -8;
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	int status;
	char filename[64];
	int index = 0;
	char f_num[15];
	FILE *jpg_file;
	int jpg_size;
	int cnt, ret;
	int fd = -1;
	char * wbuf = "hello";
	char rbuf[10];
	struct v4l2_buffer video_buffer;
	struct pollfd fds;
	unsigned int uvc_img_file_num = 0;
	char path[64];
	char * logical_drv = "/mnt/name/";

	if(argc < 2 ){
		printf("Error: no argument\n");
		return -1;
	}

	/* init vido device */
	status = video_device_init(argv[1]);
	if(status){
		printf("video_device_init = %d\n", status);
		return -1;
	}
	
	fds.fd = uvc_video_fd;
	fds.events = POLLIN;

	while(1){
		poll(&fds, 1, -1);
		video_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		video_buffer.memory = V4L2_MEMORY_MMAP;
		
		status = ioctl(uvc_video_fd, VIDIOC_DQBUF, &video_buffer);
		printf("image length: %d\n", video_buffer.bytesused);

		memset(filename, 0, 64);
		sprintf(filename, "img");
		sprintf(f_num, "%d", uvc_img_file_num ++);
		strcat(filename, f_num);
		strcat(filename, ".jpeg");
		strcpy(path, logical_drv);
		sprintf(&path[strlen(path)], "%s", filename);

		printf("%s\n\n", path);

		jpg_file = fopen(path, "wb");
		status = fwrite(image_buffer[video_buffer.index], video_buffer.bytesused, 1, jpg_file);
		fflush(jpg_file);
		status = fclose(jpg_file);

		ioctl(uvc_video_fd, VIDIOC_QBUF, &video_buffer);
	}
	
	return 0;
}

