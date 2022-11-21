#ifndef ESPI2S_H
#define ESPI2S_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <driver/i2s_std.h>
#include <driver/gpio.h>

namespace I2S
{

enum i2s_err
{
    I2S_BUS_NOT_INITIALIZED = -501,
    I2S_BUS_NOT_STARTED = -502,
    NUM_BYTES_WRITTEN_MISMATCH = -503,
    NUM_BYTES_READ_MISMATCH = -504,
    NON_MULTIPLE_BYTE_COUNT = -505,
};

class Bus
{

protected:

    bool
        _initialized,
        _started;
    size_t
        _numBytesWritten,
        _numBytesRead,
        _numTicksToWait;
    i2s_chan_handle_t
        _txHandle,
        _rxHandle;
    i2s_chan_config_t _channelConfig;
    i2s_std_config_t _stdConfig;

    virtual void _initialize();
    virtual void _disable();
    virtual void _enable();

public:

    Bus();
    Bus(const Bus& obj);
    ~Bus();
    
    virtual void set_buffer_length(int length, int count);
    virtual void set_bit_depth(uint16_t bitsPerSample);
    virtual void set_sample_rate(uint32_t samplerate);
    virtual void set_channels(uint16_t channels);
    virtual void set_i2s_bus_num(int num);
    virtual void set_master();
    virtual void set_slave();
    virtual void set_pin_master_clock(int pin = 0);
    virtual void set_pin_bit_clock(int pin);
    virtual void set_pin_word_select(int pin);
    virtual void set_pin_data_out(int pin);
    virtual void set_pin_data_in(int pin);
    virtual void set_invert_master_clock(bool invert = false);
    virtual void set_invert_bit_clock(bool invert = false);
    virtual void set_invert_word_select(bool invert = false);
    virtual void set_auto_clear(bool clear);
    
    virtual void start();
    virtual void stop();
    virtual void close();
    
    virtual void write_bytes(const void* data, int_fast32_t numBytes);
    template <typename T>
    void write(std::vector<T>* data, int_fast32_t length);
    
    virtual void read_bytes(void* data, int_fast32_t numBytes);
    template <typename T>
    void read(std::vector<T>* data, int_fast32_t length);

};

};

#endif