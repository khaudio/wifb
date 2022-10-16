#include "espi2s.h"

using namespace I2S;

Bus::Bus() :
_initialized(false),
_started(false),
_numBytesWritten(0),
_numBytesRead(0),
_numTicksToWait(100)
{
    this->_channelConfig = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    this->_channelConfig.dma_desc_num = 6;
    this->_channelConfig.dma_frame_num = 384;
    i2s_new_channel(&(this->_channelConfig), &(this->_txHandle), &(this->_rxHandle));
    this->_stdConfig.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000);
    this->_stdConfig.slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_32BIT,
            I2S_SLOT_MODE_MONO
        );
    set_invert_master_clock(false);
    set_invert_bit_clock(false);
    set_invert_word_select(false);
    set_auto_clear(true);
}

Bus::Bus(const Bus& obj) :
_initialized(obj._initialized),
_started(obj._started),
_numBytesWritten(obj._numBytesWritten),
_numBytesRead(obj._numBytesRead),
_numTicksToWait(obj._numTicksToWait),
_channelConfig(obj._channelConfig),
_stdConfig(obj._stdConfig)
{
}

Bus::~Bus()
{
    close();
}

void Bus::_initialize()
{
    if (this->_initialized) return;
    i2s_channel_init_std_mode(this->_txHandle, &this->_stdConfig);
    i2s_channel_init_std_mode(this->_rxHandle, &this->_stdConfig);
    this->_initialized = true;
}

void Bus::_disable()
{
    i2s_channel_disable(this->_txHandle);
    i2s_channel_disable(this->_rxHandle);
}

void Bus::_enable()
{
    i2s_channel_enable(this->_txHandle);
    i2s_channel_enable(this->_rxHandle);
}

void Bus::set_buffer_length(int length)
{
    /* Sets DMA buffer length */
    #ifdef _DEBUG
    if (
            (this->_stdConfig.slot_cfg.data_bit_width == I2S_DATA_BIT_WIDTH_24BIT)
            && (length % 3)
        ) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    this->_channelConfig.dma_frame_num = length;
}

void Bus::set_bit_depth(uint16_t bitsPerSample)
{
    _disable();
    if (bitsPerSample == 8)
    {
        this->_stdConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_8BIT;
        this->_stdConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_8BIT;
        this->_stdConfig.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
    }
    else if (bitsPerSample == 16)
    {
        this->_stdConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_16BIT;
        this->_stdConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_16BIT;
        this->_stdConfig.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
    }
    else if (bitsPerSample == 24)
    {
        this->_stdConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_24BIT;
        this->_stdConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_24BIT;
        this->_stdConfig.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
    }
    else if (bitsPerSample == 32)
    {
        this->_stdConfig.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
        this->_stdConfig.slot_cfg.slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT;
        this->_stdConfig.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
    }
    if (this->_initialized) _enable();
}

void Bus::set_sample_rate(uint32_t samplerate)
{
    _disable();
    this->_stdConfig.clk_cfg.sample_rate_hz = samplerate;
    i2s_channel_reconfig_std_clock(
            this->_txHandle,
            &(this->_stdConfig.clk_cfg)
        );
    i2s_channel_reconfig_std_clock(
            this->_rxHandle,
            &(this->_stdConfig.clk_cfg)
        );
    if (this->_initialized) _enable();
}

void Bus::set_channels(uint16_t channels)
{
    _disable();
    if (channels == 1)
    {
        this->_stdConfig.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO;
    }
    else if (channels == 2)
    {
        this->_stdConfig.slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    }
    #ifdef _DEBUG
    else
    {
        throw std::out_of_range("Channels must be 1 <= channels <= 2");
    }
    #endif
    i2s_channel_reconfig_std_slot(
            this->_txHandle,
            &(this->_stdConfig.slot_cfg)
        );
    i2s_channel_reconfig_std_slot(
            this->_rxHandle,
            &(this->_stdConfig.slot_cfg)
        );
    if (this->_initialized) _enable();
}

void Bus::set_i2s_bus_num(int num)
{
    #ifdef _DEBUG
    if ((num < 0) || (num > 1))
    {
        throw std::out_of_range("I2S bus number must be 0 or 1");
    }
    #endif
    this->_channelConfig.id = (num ? I2S_NUM_0 : I2S_NUM_1);
}

void Bus::set_master()
{
    this->_channelConfig.role = I2S_ROLE_MASTER;
}

void Bus::set_slave()
{
    this->_channelConfig.role = I2S_ROLE_SLAVE;
}

void Bus::set_pin_master_clock(int pin)
{
    this->_stdConfig.gpio_cfg.mclk = gpio_num_t(pin);
}

void Bus::set_pin_bit_clock(int pin)
{
    this->_stdConfig.gpio_cfg.bclk = gpio_num_t(pin);
}

void Bus::set_pin_word_select(int pin)
{
    this->_stdConfig.gpio_cfg.ws = gpio_num_t(pin);
}

void Bus::set_pin_data_out(int pin)
{
    this->_stdConfig.gpio_cfg.dout = gpio_num_t(pin);
}

void Bus::set_pin_data_in(int pin)
{
    this->_stdConfig.gpio_cfg.din = gpio_num_t(pin);
}

void Bus::set_invert_master_clock(bool invert)
{
    this->_stdConfig.gpio_cfg.invert_flags.mclk_inv = invert;
}

void Bus::set_invert_bit_clock(bool invert)
{
    this->_stdConfig.gpio_cfg.invert_flags.bclk_inv = invert;
}

void Bus::set_invert_word_select(bool invert)
{
    this->_stdConfig.gpio_cfg.invert_flags.ws_inv = invert;
}

void Bus::set_auto_clear(bool clear)
{
    this->_channelConfig.auto_clear = clear;
}

void Bus::start()
{
    _initialize();
    _enable();
}

void Bus::stop()
{
    _disable();
}

void Bus::close()
{
    _disable();
    i2s_del_channel(this->_txHandle);
    i2s_del_channel(this->_rxHandle);
}

void Bus::write_bytes(const void* data, int_fast32_t numBytes)
{
    i2s_channel_write(
            this->_txHandle,
            data,
            numBytes,
            &(this->_numBytesWritten),
            this->_numTicksToWait
        );
    #ifdef _DEBUG
    if (numBytes != this->_numBytesWritten)
    {
        std::cerr << "Error: " << this->_numBytesWritten;
        std::cerr << " of " << numBytes << " written\n";
    }
    #endif
}

template <typename T>
inline void Bus::write(std::vector<T>* data, int_fast32_t length)
{
    constexpr int_fast32_t typeSize(sizeof(T));
    write_bytes(reinterpret_cast<const void*>(&((*data)[0])), length * typeSize);
}

void Bus::read_bytes(void* data, int_fast32_t numBytes)
{
    i2s_channel_read(
        this->_rxHandle,
        data,
        numBytes,
        &(this->_numBytesRead),
        this->_numTicksToWait
    );
    #ifdef _DEBUG
    if (numBytes != this->_numBytesRead)
    {
        std::cerr << "Error: " << this->_numBytesRead;
        std::cerr << " of " << numBytes << " read\n";
    }
    #endif
}

template <typename T>
void Bus::read(std::vector<T>* data, int_fast32_t length)
{
    constexpr int_fast32_t typeSize(sizeof(T));
    read_bytes(reinterpret_cast<void*>(&((*data)[0])), length * typeSize);
}

template void Bus::write<int8_t>(std::vector<int8_t>*, int_fast32_t);
template void Bus::write<uint8_t>(std::vector<uint8_t>*, int_fast32_t);
template void Bus::write<int16_t>(std::vector<int16_t>*, int_fast32_t);
template void Bus::write<uint16_t>(std::vector<uint16_t>*, int_fast32_t);
template void Bus::write<int32_t>(std::vector<int32_t>*, int_fast32_t);
template void Bus::write<uint32_t>(std::vector<uint32_t>*, int_fast32_t);

template void Bus::write<int_fast8_t>(std::vector<int_fast8_t>*, int_fast32_t);
template void Bus::write<uint_fast8_t>(std::vector<uint_fast8_t>*, int_fast32_t);

template void Bus::read<int8_t>(std::vector<int8_t>*, int_fast32_t);
template void Bus::read<uint8_t>(std::vector<uint8_t>*, int_fast32_t);
template void Bus::read<int16_t>(std::vector<int16_t>*, int_fast32_t);
template void Bus::read<uint16_t>(std::vector<uint16_t>*, int_fast32_t);
template void Bus::read<int32_t>(std::vector<int32_t>*, int_fast32_t);
template void Bus::read<uint32_t>(std::vector<uint32_t>*, int_fast32_t);

template void Bus::read<int_fast8_t>(std::vector<int_fast8_t>*, int_fast32_t);
template void Bus::read<uint_fast8_t>(std::vector<uint_fast8_t>*, int_fast32_t);