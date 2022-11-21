#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <vector>
#include <map>
#include <stdexcept>
#include <type_traits>

/* Whether the first read index
will be advanced automatically
after the first write or whether it
will be rotated manually.  Enabling
this directive adds an additional check
during each write buffer rotation.
Disable for maximum performance. */
#ifndef RINGBUFF_AUTO_FIRST_ROTATE
#define RINGBUFF_AUTO_FIRST_ROTATE          1
#endif

namespace Buffer
{

enum ringbuffer_err
{
    RING_SIZE_TOO_SHORT = -30,
    UNEVEN_BUFFER_LENGTH = -31,
    NON_MULTIPLE_BUFFER_LENGTH = -32,
    BUFFER_LENGTH_TOO_LONG = -40,
    BUFFER_NOT_INITIALIZED = -50,
    BUFFER_ADDR_NOT_FOUND = -60,
    WRITE_BUFFER_OVERRUN = -71,
    WRITE_BUFFER_UNDERRUN = -72,
    READ_BUFFER_OVERRUN = -73,
    READ_BUFFER_UNDERRUN = -74,
    PROCESSING_BUFFER_OVERRUN = -75,
    PROCESSING_BUFFER_UNDERRUN = -76,
    VALUE_MUST_BE_NONZERO = -80,
    NON_MULTIPLE_BYTE_COUNT = -81,
    BUFFER_COUNT_TOO_SHORT = -90,
    SIZE_NOT_SET = -91,
};

template <typename T, typename I>
class Base;

template <typename T, typename I>
class RingBuffer;

template <typename T>
class NonAtomicRingBuffer;

template <typename T>
class AtomicRingBuffer;

template <typename T, typename I>
class MultiRingBuffer;

template <typename T>
class NonAtomicMultiRingBuffer;

template <typename T>
class AtomicMultiRingBuffer;

template <typename T>
class AtomicMultiReadRingBuffer;


template <typename T, typename I>
class Base
{

    friend class RingBuffer<T, I>;
    friend class MultiRingBuffer<T, I>;

protected:

    /* Rotate write index before first write */
    bool _firstWritten;

    int_fast8_t _ringLength;

    int_fast32_t
        _buffered,
        _samplesWritten,
        _samplesUnwritten,
        _samplesProcessed,
        _samplesUnread,
        _bufferLength,
        _bytesPerBuffer,
        _totalWritableLength,
        _totalRingSampleLength;

public:

    I
        readIndex{0},
        writeIndex{0},
        processingIndex{0};

public:

    Base();
    Base(int_fast32_t bufferSize, int_fast8_t ringSize);
    Base(const Base& obj);

    virtual ~Base();

    /* Indicator as to whether ring
    and buffer vectors have been allocated */
    virtual bool size_is_set() const;

    /* Sets ring and buffer size */
    virtual void set_size(int_fast32_t bufferSize, int_fast8_t ringSize);

    /* Total number of samples that can fit in all rings */
    virtual int_fast32_t size() const;

    /* Total number of bytes that can fit in all rings */
    virtual int_fast32_t total_size() const;

    /* Number of buffers in the ring */
    virtual int_fast8_t ring_length() const;

    /* Length in samples of each buffer in the ring */
    virtual int_fast32_t buffer_length() const;

    /* Number of bytes per sample */
    constexpr static int_fast32_t bytes_per_sample();

    /* Size in bytes of each buffer in the ring */
    virtual int_fast32_t bytes_per_buffer() const;

    /* Checks bounds to prevent buffer collisions */
    virtual bool is_writable() const;

    /* Reset all counters and indices */
    virtual void reset();

/*                          Sample Counters                         */

    /* Total number of unread samples buffered,
    excluding the current write buffer */
    virtual int_fast32_t buffered() const;

    /* Total number of unread bytes buffered,
    excluding the current write buffer */
    virtual int_fast32_t bytes_buffered() const;

    /* Total number of samples unbuffered,
    excluding current read buffer */
    virtual int_fast32_t available() const;

    /* Total number of bytes unbuffered,
    excluding current read buffer */
    virtual int_fast32_t bytes_available() const;

    /* Total number of samples processed */
    virtual int_fast32_t processed() const;

    /* Total number of bytes processed */
    virtual int_fast32_t bytes_processed() const;

    /* Total number of samples written but not processed */
    virtual int_fast32_t unprocessed() const;

    /* Total number of bytes written but not processed */
    virtual int_fast32_t bytes_unprocessed() const;

    /* Total number of samples unread
    in the current read buffer */
    virtual int_fast32_t unread() const;

    /* Total number of bytes unread
    in the current read buffer */
    virtual int_fast32_t bytes_unread() const;

    /* Total number of samples unwritten
    in the current write buffer */
    virtual int_fast32_t unwritten() const;

    /* Total number of bytes unwritten
    in the current write buffer */
    virtual int_fast32_t bytes_unwritten() const;

/*                          Buffer Counters                         */

    /* Total number of unread readable buffers.
    If partial rotations are used,
    this counter may be inaccurate */
    virtual int_fast32_t buffers_buffered() const;

    /* Total number of writable buffers.
    If partial rotations are used,
    this counter may be inaccurate */
    virtual int_fast32_t buffers_available() const;

    /* Total number of processed readable buffers
    If partial rotations are used,
    this counter may be inaccurate */
    virtual int_fast32_t buffers_processed() const;

/*                               Read                               */

    /* Rotate read index
    without changing sample counters */
    virtual void rotate_read_index();

    /* Rotates read buffer */
    virtual void rotate_read_buffer();

    /* Rotates read buffer after reading only a specified
    number of samples instead of the entire buffer */
    virtual void rotate_partial_read(int_fast32_t length);

    /* Update counters with number of samples read externally */
    virtual void report_read_samples(int_fast32_t length);

    /* Update counters with number of bytes read externally */
    virtual void report_read_bytes(int_fast32_t numBytes);

/*                               Write                              */

    /* Rotate write index
    without changing sample counters */
    virtual void rotate_write_index();

    /* Rotates write buffer
    and forces read buffer forward if overrun */
    virtual void rotate_write_buffer(bool force = false);

    /* Rotates write buffer after writing only a specified
    number of samples instead of the entire buffer */
    virtual void rotate_partial_write(
            int_fast32_t length,
            bool force = false
        );

    /* Update counters with number of samples written externally */
    virtual void report_written_samples(int_fast32_t length);

    /* Update counters with number of bytes written externally */
    virtual void report_written_bytes(int_fast32_t numBytes);

/*                             Transform                            */

    /* Rotate processing index
    without changing sample counters */
    virtual void rotate_processing_index();

    /* Updates counters and advances
    the processing index */
    virtual void rotate_processing_buffer();

    /* Rotates processing buffer after processing only
    a specified number of samples instead of the entire buffer */
    virtual void rotate_partial_processing(int_fast32_t length);

    /* Update counters with number of samples processed */
    virtual void report_processed_samples(int_fast32_t length);

    /* Update counters with number of bytes processed */
    virtual void report_processed_bytes(int_fast32_t length);

};

template <typename T, typename I>
class RingBuffer :
public Base<T, I>
{

public:

    std::vector<std::vector<T>> ring;

    RingBuffer();
    RingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize);
    RingBuffer(const RingBuffer& obj);

    virtual ~RingBuffer();

    /* Sets ring and buffer size and allocates vectors */
    virtual void set_size(int_fast32_t bufferSize, int_fast8_t ringSize);

    /* Fill all the buffers with value */
    virtual void fill(T value = 0);

    /* Returns ring index for buffer at pointer */
    virtual int_fast8_t get_ring_index(std::vector<T>* bufferPtr);

    /* Returns ring index for buffer beginning at pointer */
    virtual int_fast8_t get_ring_index(uint8_t* bufferPtr);

/*                               Read                               */

protected:

    /* Returns current read buffer */
    virtual std::vector<T> _read();

public:

    /* Returns current read buffer and rotates */
    virtual std::vector<T> read();

    /* Copies samples from read buffer to data pointer */
    virtual void read_samples(T* data, int_fast32_t length);

    /* Copies bytes from read buffer to data pointer */
    virtual void read_bytes(uint8_t* data, int_fast32_t numBytes);

    /* Returns pointer to current read buffer */
    virtual std::vector<T>* get_read_buffer();

    /* Returns pointer to first sample
    of current read buffer */
    virtual T* get_read_buffer_sample();

    /* Returns pointer to first byte
    of current read buffer */
    virtual uint8_t* get_read_buffer_byte();

    /* Returns pointer to sample in buffer
    at current read sample index */
    virtual T* get_read_sample();

    /* Returns pointer to byte in buffer
    at current read sample index */
    virtual uint8_t* get_read_byte();

/*                               Write                              */

public:

    /* Writes a single sample.
    If forced, unread data will be overwritten. */
    virtual int_fast32_t write(T data, bool force = false);

    /* Writes along ring and returns total number of samples written.
    If forced, unread data will be overwritten. */
    virtual int_fast32_t write(std::vector<T> data, bool force = false);

    /* Writes specified number of samples.
    If forced, unread data will be overwritten. */
    virtual int_fast32_t write_samples(
            T* data,
            int_fast32_t length,
            bool force = false
        );

    /* Returns number of bytes written.
    If forced, unread data will be overwritten. */
    virtual int_fast32_t write_bytes(
            uint8_t* data,
            int_fast32_t numBytes,
            bool force = false
        );

    /* Returns pointer to current write buffer */
    virtual std::vector<T>* get_write_buffer();

    /* Returns pointer to first sample
    of current write buffer */
    virtual T* get_write_buffer_sample();

    /* Returns pointer to first byte
    of current write buffer */
    virtual uint8_t* get_write_buffer_byte();

    /* Returns pointer to sample in buffer
    at current write sample index */
    virtual T* get_write_sample();

    /* Returns pointer to byte in buffer
    at current write sample index */
    virtual uint8_t* get_write_byte();

/*                             Transform                            */

    /* Returns pointer to current processing buffer */
    virtual std::vector<T>* get_processing_buffer();

    /* Returns pointer to first sample
    of current processing buffer */
    virtual T* get_processing_buffer_sample();

    /* Returns pointer to first byte
    of current processing buffer */
    virtual uint8_t* get_processing_buffer_byte();

    /* Returns pointer to sample in buffer
    at current processing sample index */
    virtual T* get_processing_sample();

    /* Returns pointer to byte in buffer
    at current processing sample index */
    virtual uint8_t* get_processing_byte();

};

template <typename T>
class NonAtomicRingBuffer :
public RingBuffer<T, int_fast8_t>
{
public:

    NonAtomicRingBuffer();
    NonAtomicRingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize);
    NonAtomicRingBuffer(const NonAtomicRingBuffer& obj);

    virtual ~NonAtomicRingBuffer();

};

template <typename T>
class AtomicRingBuffer :
public RingBuffer<T, std::atomic_int_fast8_t>
{
public:

    AtomicRingBuffer();
    AtomicRingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize);
    AtomicRingBuffer(const AtomicRingBuffer& obj);
    
    virtual ~AtomicRingBuffer();

};


template <typename T>
class AtomicMultiReadRingBuffer :
public AtomicRingBuffer<T>
{
protected:

    std::atomic_int_fast8_t
        _readCounter{0},
        _numReaders{1};

protected:

    /* Increments read counter and returns true
    if reported read by all readers */
    virtual bool _increment_read_counter();

public:

    AtomicMultiReadRingBuffer();
    AtomicMultiReadRingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize);
    AtomicMultiReadRingBuffer(const AtomicMultiReadRingBuffer& obj);

    virtual ~AtomicMultiReadRingBuffer();

    virtual void set_num_readers(int_fast8_t numReaders = 1);
    virtual int_fast8_t num_readers() const;

/*                               Read                               */

public:

    /* Returns current read buffer and reports read */
    virtual std::vector<T> read() override;

    /* Update counters with number of samples read externally */
    virtual void report_read_samples(int_fast32_t length);

};

};

#endif
