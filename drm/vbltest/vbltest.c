/*
 * DRM based vblank test program
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "xf86drm.h"
#include "xf86drmMode.h"

#include "util/common.h"
#include "util/kms.h"

#include <drm_fourcc.h>

extern char *optarg;
extern int optind, opterr, optopt;
static char optstr[] = "D:M:s";

int secondary = 0;

struct vbl_info {
	unsigned int vbl_count;
	struct timeval start;
};

struct buffer_object {
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	uint32_t handle;
	uint32_t size;
	uint32_t offset;
	uint8_t *map;
	uint32_t fb_handle;
};

static struct buffer_object buf;

static int modeset_create_fb(int fd, struct buffer_object *buf)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_map_dumb mreq;
	uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};
	int ret;

	/* create dumb buffer */
	memset(&creq, 0, sizeof(creq));
	creq.width = buf->width;
	creq.height = buf->height;
	creq.bpp = 32;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		printf("DRM_IOCTL_MODE_CREATE_DUMB fail\n");
		return -1;
	}

	buf->handle = creq.handle;
	buf->pitch = creq.pitch;
	printf("pitch %d\n", buf->pitch);
	buf->size = creq.size;
	printf("size %d\n", buf->size);

	/* prepare buffer for memory mapping */
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = creq.handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		printf("DRM_IOCTL_MODE_MAP_DUMB fail\n");
		return -1;
	}

	buf->offset = mreq.offset;

	/* perform actual memory mapping */
	buf->map = mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mreq.offset);
	if (buf->map == MAP_FAILED) {
		printf("mmap fail\n");
		return -1;
	}

	/* clear the framebuffer to 0 (= full transparency in ARGB8888) */
	//memset(buf->map, 0, creq.size);
	int *buffer = (int *)buf->map;
	for (int i = 0; i < buf->width * buf->height; i++) {
		buffer[i] = 0xffff0000;
	}

	/* create framebuffer object for the dumb-buffer */
	handles[0] = creq.handle;
	pitches[0] = creq.pitch;
	offsets[0] = 0;
	ret = drmModeAddFB2(fd, buf->width, buf->height, DRM_FORMAT_ARGB8888,
						handles, pitches, offsets, &buf->fb_handle, 0);
	if (ret) {
		printf("drmModeAddFB fail\n");
		return -1;
	}

	return 0;
}

static void vblank_handler(int fd, unsigned int frame, unsigned int sec,
			   unsigned int usec, void *data)
{
	drmVBlank vbl;
	struct timeval end;
	struct vbl_info *info = data;
	double t;

	vbl.request.type = DRM_VBLANK_RELATIVE | DRM_VBLANK_EVENT;
	if (secondary)
		vbl.request.type |= DRM_VBLANK_SECONDARY;
	vbl.request.sequence = 1;
	vbl.request.signal = (unsigned long)data;

	drmWaitVBlank(fd, &vbl);

	info->vbl_count++;

	if (info->vbl_count == 60) {
		gettimeofday(&end, NULL);
		t = end.tv_sec + end.tv_usec * 1e-6 -
			(info->start.tv_sec + info->start.tv_usec * 1e-6);
		fprintf(stderr, "freq: %.02fHz\n", info->vbl_count / t);
		info->vbl_count = 0;
		info->start = end;
	}
}

static void usage(char *name)
{
	fprintf(stderr, "usage: %s [-DMs]\n", name);
	fprintf(stderr, "\n");
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -D DEVICE  open the given device\n");
	fprintf(stderr, "  -M MODULE  open the given module\n");
	fprintf(stderr, "  -s         use secondary pipe\n");
	exit(0);
}

int main(int argc, char **argv)
{
	const char *device = NULL, *module = NULL;
	int c, fd, ret;
	drmVBlank vbl;
	drmEventContext evctx;
	struct vbl_info handler_info;

	opterr = 0;
	while ((c = getopt(argc, argv, optstr)) != -1) {
		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 'M':
			module = optarg;
			break;
		case 's':
			secondary = 1;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}

	//fd = util_open(device, module);
	fd = open("/dev/dri/card0", O_RDWR, 0);
	if (fd < 0)
		return 1;

	if (ret = drmSetClientCap(fd, DRM_CLIENT_CAP_ATOMIC, 1)) {
		printf("No atomic modesetting support: %s\n", strerror(errno));
		return -1;
	}

	drmModeRes *mode_res;
	if ((mode_res = drmModeGetResources(fd)) == NULL) {
		printf("drmModeGetResources() failed\n");
		return -1;
	}

	drmModeConnector *conn = NULL;
	if ((conn = drmModeGetConnector(fd, mode_res->connectors[0])) == NULL) {
		printf("drmModeGetConnector() failed\n");
		return -1;
	}

	buf.width = conn->modes[0].hdisplay;
	buf.height = conn->modes[0].vdisplay;

	modeset_create_fb(fd, &buf);

	drmModeSetCrtc(fd, mode_res->crtcs[0], buf.fb_handle, 0, 0, &mode_res->connectors[0],
				   1, &conn->modes[0]);

	/* Get current count first */
	vbl.request.type = DRM_VBLANK_RELATIVE;
	if (secondary)
		vbl.request.type |= DRM_VBLANK_SECONDARY;
	vbl.request.sequence = 0;
	ret = drmWaitVBlank(fd, &vbl);
	if (ret != 0) {
		printf("drmWaitVBlank (relative) failed ret: %i\n", ret);
		return -1;
	}

	printf("starting count: %d\n", vbl.request.sequence);

	handler_info.vbl_count = 0;
	gettimeofday(&handler_info.start, NULL);

	/* Queue an event for frame + 1 */
	vbl.request.type = DRM_VBLANK_RELATIVE | DRM_VBLANK_EVENT;
	if (secondary)
		vbl.request.type |= DRM_VBLANK_SECONDARY;
	vbl.request.sequence = 1;
	vbl.request.signal = (unsigned long)&handler_info;
	ret = drmWaitVBlank(fd, &vbl);
	if (ret != 0) {
		printf("drmWaitVBlank (relative, event) failed ret: %i\n", ret);
		return -1;
	}

	/* Set up our event handler */
	memset(&evctx, 0, sizeof evctx);
	evctx.version = DRM_EVENT_CONTEXT_VERSION;
	evctx.vblank_handler = vblank_handler;
	evctx.page_flip_handler = NULL;

	/* Poll for events */
	while (1) {
		struct timeval timeout = { .tv_sec = 3, .tv_usec = 0 };
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(0, &fds);
		FD_SET(fd, &fds);
		ret = select(fd + 1, &fds, NULL, NULL, &timeout);

		if (ret <= 0) {
			fprintf(stderr, "select timed out or error (ret %d)\n",
				ret);
			continue;
		} else if (FD_ISSET(0, &fds)) {
			break;
		}

		ret = drmHandleEvent(fd, &evctx);
		if (ret != 0) {
			printf("drmHandleEvent failed: %i\n", ret);
			return -1;
		}
	}

	return 0;
}
