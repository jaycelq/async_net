#ifndef _ASYNC_DEF_H_
#define _ASYNC_DEF_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>

#define BENCHMARK 1
#ifdef BENCHMARK
#define LOG_TRACE(fmt, args...) {}
#define LOG_DEBUG(fmt, args...) {}
#define LOG_INFO(fmt, args...) {}
#define LOG_ERROR(fmt, args...) {}
#define LOG_FATAL(fmt, args...) {}
#else
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
#define LOG_TRACE(fmt, args...) printf("[LOG_TRACE] %s %s %s %s %d " fmt "\n", __DATE__, __TIME__, __FILENAME__, __func__, __LINE__, ##args)
#define LOG_DEBUG(fmt, args...) printf("[LOG_DEBUG] %s %s %s %s %d " fmt "\n", __DATE__, __TIME__, __FILENAME__, __func__, __LINE__, ##args)
#define LOG_INFO(fmt, args...) printf("[LOG_INFO] %s %s %s %s %d " fmt "\n", __DATE__, __TIME__, __FILENAME__, __func__, __LINE__, ##args)
#define LOG_ERROR(fmt, args...) printf("[LOG_ERROR] %s %s %s %s %d " fmt "\n", __DATE__, __TIME__, __FILENAME__, __func__, __LINE__, ##args)
#define LOG_FATAL(fmt, args...) printf("[LOG_FATAL] %s %s %s %s %d " fmt "\n", __DATE__, __TIME__, __FILENAME__, __func__, __LINE__, ##args)
#endif

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_MAX_EVENT_FD 1024

#endif
