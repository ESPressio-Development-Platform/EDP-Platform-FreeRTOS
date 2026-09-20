#pragma once

#if __has_include(<freertos/FreeRTOS.h>)

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#elif __has_include(<FreeRTOS.h>)

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#else

#error "EDP-Platform-FreeRTOS requires FreeRTOS kernel headers"

#endif
