#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <unistd.h>
#include <linux/videodev2.h>

#include "v4l2_camera.h"
#include "spi_camera.h"
#include "drm_display.h"
#include "cmd.h"
#include "libiruart.h"
#include "libiri2c.h"
#include "libirspi.h"