#ifndef ESPDELAY_H
#define ESPDELAY_H

#include <chrono>
#include <thread>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#if _DEBUG

#define DELAY_COUNTER_INT(x)                int delayCounter(x)
#define DELAY_TICKS_AT_COUNT(x)             delay_ticks_at_count(&delayCounter, x, 1)
#define DELAY_MS_AT_COUNT(x)                delay_ms_at_count(&delayCounter, x, 1)
#define DECLARE_AND_DELAY_AT_COUNT(x)       DELAY_COUNTER_INT(0); DELAY_TICKS_AT_COUNT(x)

#else

#define DELAY_COUNTER_INT(x)
#define DELAY_TICKS_AT_COUNT(x)
#define DELAY_MS_AT_COUNT(x)
#define DECLARE_AND_DELAY_AT_COUNT(x)

#endif

void delay_ticks(int numTicks);
void delay_ms(int ms);
void delay_ticks_at_count(int* counter, int threshold, int numTicks);
void delay_ms_count(int* counter, int threshold, int ms);

#endif
