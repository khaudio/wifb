#ifndef ESPDELAY_H
#define ESPDELAY_H

#include <chrono>
#include <thread>

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

void delay_ticks(int numTicks);
void delay_ms(int ms);
void delay_ticks_count(int* counter, int threshold, int numTicks);
void delay_ms_count(int* counter, int threshold, int numTicks);

#endif
