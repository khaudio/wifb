#include "espdelay.h"

void delay_ticks(int numTicks)
{
    #ifdef ESP_PLATFORM
    /* Delay for ticks */
    vTaskDelay(numTicks);
    #else
    /* Delay for nanoseconds */
    std::this_thread::sleep_for(std::chrono::nanoseconds(numTicks));
    #endif
}

void delay_ms(int ms)
{
    /* Delay for milliseconds */
    #ifdef ESP_PLATFORM
    vTaskDelay(ms / portTICK_PERIOD_MS);
    #else
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    #endif
}

void delay_ticks_count(int* counter, int threshold, int numTicks)
{
    /* Delay after a specified number of external iterations */
    if (++(*counter) %= threshold) return;
    delay_ticks(numTicks);
}

void delay_ms_count(int* counter, int threshold, int numTicks)
{
    /* Delay after a specified number of external iterations */
    if (++(*counter) %= threshold) return;
    delay_ms(numTicks);
}
