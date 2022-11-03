#ifndef MULTIBUFFER_H
#define MULTIBUFFER_H

#include "ringbuffer.h"

namespace Buffer
{

template <typename T, typename I>
class MultiRingBuffer :
public Buffer::Base<T, I>
{

protected:

    int_fast8_t _numBuffers;

public:

    std::vector<RingBuffer<T, I>> buffers;

public:

    MultiRingBuffer();
    MultiRingBuffer(
            int_fast32_t bufferSize,
            int_fast8_t ringSize,
            int_fast8_t numBuffers
        );
    MultiRingBuffer(const MultiRingBuffer& obj);

    virtual ~MultiRingBuffer();

    bool size_is_set() const override;

    virtual void set_size(
            int_fast32_t bufferSize,
            int_fast8_t ringSize,
            int_fast8_t numBuffers
        );

    int_fast32_t size() const override;
    int_fast32_t total_size() const override;
    int_fast8_t num_buffers() const;

    void fill(T value = 0);
    void reset() override;

    auto get_buffer(int_fast8_t bufferIndex);

/*                          Sample Counters                         */

protected:

    /* Updates global sample counters
    with the maximum value that can be applied
    to all sub buffers; i.e., the greatest sample count
    that can be written or read without overruns */
    virtual void _update_sample_counters();
    virtual void _increment_counters();

public:

    /* Updates sample counters and rotate buffers */
    virtual void update();

    /* Updates counters and return maximum common number
    of samples buffered across all sub buffers */
    int_fast32_t buffered();
    
    /* Updates counters and return maximum common number
    of samples processed across all sub buffers */
    int_fast32_t processed();

/*                               Read                               */

public:

    /* Reads an equal number of samples from each buffer
    and returns a vector of interleaved samples
    with length equal to buffer length */
    virtual std::vector<T> read_interleaved();

    /* Reads an equal number of samples from each buffer
    to a vector of interleaved samples with length equal
    to buffer length */
    virtual void read_interleaved(std::vector<T>* data);

    /* Copies specified number of samples divided among buffers
    and interleaved to data pointer */
    virtual void read_samples_interleaved(T* data, int_fast32_t length);

    /* Copies specified number of bytes divided among buffers
    and interleaved to data pointer */
    virtual void read_bytes_interleaved(uint8_t* data, int_fast32_t numBytes);

    /* Read an equal number of samples from each buffer
    and returns a vector of concatenated samples
    with length equal to buffer length */
    virtual std::vector<T> read_concatenated();

    /* Reads an equal number of samples from each buffer
    to a vector of concatenated samples with length equal
    to buffer length */
    virtual void read_concatenated(std::vector<T>* data);
    
    /* Copies specified number of samples divided among buffers
    and concatenated to data pointer */
    virtual void read_samples_concatenated(T* data, int_fast32_t length);

    /* Copies specified number of bytes divided among buffers
    and concatenated to data pointer */
    virtual void read_bytes_concatenated(uint8_t* data, int_fast32_t numBytes);

/*                               Write                              */

public:

    /* Writes a single sample to each buffer.
    If forced, unread data will be overwritten. */
    int_fast32_t write(T data, bool force = false);

    /* Writes to each buffer along respective rings
    and returns lowest common number of samples
    written to each. If forced, unread data
    will be overwritten. */
    int_fast32_t write(std::vector<T> data, bool force = false);

    /* Writes specified number of samples to each buffer
    and return lowest common number of samples
    written to each. If forced, unread data
    will be overwritten. */
    int_fast32_t write_samples(
            T* data,
            int_fast32_t length,
            bool force = false
        );
    
    /* Writes specified number of bytes to each buffer
    and return lowest common number of bytes
    written to each. If forced, unread data
    will be overwritten. */
    int_fast32_t write_bytes(
            uint8_t* data,
            int_fast32_t numBytes,
            bool force = false
        );

};

template <typename T>
class NonAtomicMultiRingBuffer :
public MultiRingBuffer<T, int_fast8_t>
{
public:

    NonAtomicMultiRingBuffer();
    NonAtomicMultiRingBuffer(
            int_fast32_t bufferSize,
            int_fast8_t ringSize,
            int_fast8_t numBuffers
        );
    NonAtomicMultiRingBuffer(const NonAtomicMultiRingBuffer& obj);

    virtual ~NonAtomicMultiRingBuffer();

};

template <typename T>
class AtomicMultiRingBuffer :
public MultiRingBuffer<T, std::atomic_int_fast8_t>
{
public:

    AtomicMultiRingBuffer();
    AtomicMultiRingBuffer(
            int_fast32_t bufferSize,
            int_fast8_t ringSize,
            int_fast8_t numBuffers
        );
    AtomicMultiRingBuffer(const AtomicMultiRingBuffer& obj);

    virtual ~AtomicMultiRingBuffer();

};

};

#endif
