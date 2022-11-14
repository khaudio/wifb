#ifndef ESPDELAY_H
#define ESPDELAY_H

#include <chrono>
#include <thread>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#if _DEBUG

#define DELAY_COUNTER_INT(x)        int delayCounter(x)
#define DELAY_TICKS_COUNT(x)        delay_ticks_count(&delayCounter, x, 1)
#define DELAY_MS_COUNT(x)           delay_ms_count(&delayCounter, x, 1)

#else

#define DELAY_COUNTER_INT(x)
#define DELAY_TICKS_COUNT(x)

#endif

void delay_ticks(int numTicks);
void delay_ms(int ms);
void delay_ticks_count(int* counter, int threshold, int numTicks);
void delay_ms_count(int* counter, int threshold, int ms);

#endif
