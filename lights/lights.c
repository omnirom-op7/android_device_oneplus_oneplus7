/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (C) 2014 The Linux Foundation. All rights reserved.
 * Copyright (C) 2016 The CyanogenMod Project
 * Copyright (C) 2017 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "lights"
//#define LOG_NDEBUG 0

#include <cutils/log.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

#define LCD_BRIGHTNESS_FILE "/sys/class/backlight/panel0-backlight/brightness"
#define LCD_MAX_BRIGHTNESS_FILE "/sys/class/backlight/panel0-backlight/max_brightness"

#define DEFAULT_MAX_BRIGHTNESS 255
int max_brightness;

/**
 * Device methods
 */

static void init_globals(void)
{
    // Init the mutex
    pthread_mutex_init(&g_lock, NULL);
}

static int read_int(char const* path)
{
    int fd, len;
    int num_bytes = 10;
    char buf[11];
    int retval;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        ALOGE("%s: failed to open %s\n", __func__, path);
        goto fail;
    }

    len = read(fd, buf, num_bytes - 1);
    if (len < 0) {
        ALOGE("%s: failed to read from %s\n", __func__, path);
        goto fail;
    }

    buf[len] = '\0';
    close(fd);

    // no endptr, decimal base
    retval = strtol(buf, NULL, 10);
    return retval == 0 ? -1 : retval;

fail:
    if (fd >= 0)
        close(fd);
    return -1;
}

static int write_int(char const* path, int value)
{
    int fd;
    static int already_warned = 0;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[20];
        int bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
        ssize_t amt = write(fd, buffer, (size_t)bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        if (already_warned == 0) {
            ALOGE("%s: failed to open %s\n", __func__, path);
            already_warned = 1;
        }
        return -errno;
    }
}

static int rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;

    return ((77 * ((color >> 16) & 0x00ff))
            + (150 * ((color >> 8) & 0x00ff)) + (29 * (color & 0x00ff))) >> 8;
}

static int set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    int brightness = rgb_to_brightness(state);

    if (!dev)
        return -1;

    // If max panel brightness is not the default (255),
    // apply linear scaling across the accepted range.
    if (max_brightness != DEFAULT_MAX_BRIGHTNESS) {
        int old_brightness = brightness;
        brightness = brightness * max_brightness / DEFAULT_MAX_BRIGHTNESS;
        ALOGV("%s: scaling brightness %d => %d\n", __func__, old_brightness, brightness);
    }

    pthread_mutex_lock(&g_lock);
    err = write_int(LCD_BRIGHTNESS_FILE, brightness);
    pthread_mutex_unlock(&g_lock);
    return err;
}

/** Close the lights device */
static int close_lights(struct light_device_t *dev)
{
    if (dev)
        free(dev);
    return 0;
}

/**
 * Module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name))
        set_light = set_light_backlight;
    else
        return -EINVAL;

    max_brightness = read_int(LCD_MAX_BRIGHTNESS_FILE);
    if (max_brightness < 0) {
        ALOGE("%s: failed to read max panel brightness, fallback to 255!\n", __func__);
        max_brightness = DEFAULT_MAX_BRIGHTNESS;
    }

    pthread_once(&g_init, init_globals);

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));

    if (!dev)
        return -ENOMEM;

    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;
    return 0;
}

static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

/*
 * The lights Module
 */
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "MSM8996 Lights Module",
    .author = "The CyanogenMod Project",
    .methods = &lights_module_methods,
};
