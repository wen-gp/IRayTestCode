#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <unistd.h>
#include <linux/videodev2.h>

#include "uvc_camera.h"
#include "cmd.h"
#include "libiruart.h"