#pragma once

#include <cerrno>
#include <cstdio>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOGD(fmt, ...)  printf("[DEBUG] (%s) " fmt "\n", __FILENAME__, ##__VA_ARGS__)
#define LOGI(fmt, ...)  printf("[ INFO] (%s) " fmt "\n", __FILENAME__, ##__VA_ARGS__)
#define LOGE(fmt, ...)  printf("[ERROR] (%s) " fmt "\n", __FILENAME__, ##__VA_ARGS__)
#define PLOGE(fmt, ...) printf("[ERROR] (%s) ENO: %d %s " fmt "\n", __FILENAME__, errno, strerror(errno), ##__VA_ARGS__)
