#include "ringbuffer.h"

using namespace Buffer;

template <typename T>
RingBuffer<T>::RingBuffer() :
_ringLength(0),
_buffered(0),
_samplesWritten(0),
_samplesUnwritten(0),
_samplesProcessed(0),
_samplesUnread(0),
_bufferLength(0),
_bytesPerBuffer(0),
_totalWritableLength(0),
_totalRingSampleLength(0),
readIndex(0),
writeIndex(0),
processingIndex(0)
{
}

template <typename T>
RingBuffer<T>::RingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    set_size(bufferSize, ringSize);
}

template <typename T>
RingBuffer<T>::RingBuffer(const RingBuffer& obj) :
_ringLength(obj._ringLength),
_buffered(obj._buffered),
_samplesWritten(obj._samplesWritten),
_samplesUnwritten(obj._samplesUnwritten),
_samplesProcessed(obj._samplesProcessed),
_samplesUnread(obj._samplesUnread),
_bufferLength(obj._bufferLength),
_bytesPerBuffer(obj._bytesPerBuffer),
_totalWritableLength(obj._totalWritableLength),
_totalRingSampleLength(obj._totalRingSampleLength),
readIndex(obj.readIndex),
writeIndex(obj.writeIndex),
processingIndex(obj.processingIndex)
{
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
inline bool RingBuffer<T>::_size_is_set() const
{
    return ((this->_bufferLength > 0) && (this->_ringLength > 0));
}

template <typename T>
void RingBuffer<T>::set_size(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    #ifdef _DEBUG
    /* There must be multiple buffers to rotate */
    if (ringSize < 2) throw RING_SIZE_TOO_SHORT;

    /* Assert that total ring sample length is less than datatype max */
    if (bufferSize > (
            std::numeric_limits<int_fast32_t>::max()
            / static_cast<int_fast32_t>(ringSize)
        ))
    {
        throw BUFFER_LENGTH_TOO_LONG;
    }
    #endif

    this->_ringLength = ringSize;
    this->_bufferLength = bufferSize;
    this->_totalRingSampleLength = this->_ringLength * this->_bufferLength;
    this->_totalWritableLength = (
            this->_totalRingSampleLength
            - this->_bufferLength
        );
    this->_bytesPerBuffer = this->_bufferLength * bytesPerSample;
    this->_samplesUnwritten = this->_bufferLength;
    this->_samplesWritten = 0;
    this->_samplesUnread = this->_bufferLength;
    this->_buffered = 0;
    this->ring = std::vector<std::vector<T>>();
    for (int_fast8_t i(0); i < this->_ringLength; ++i)
    {
        this->ring.emplace_back(std::vector<T>());
        this->ring[i].reserve(this->_bufferLength);
        for (int_fast32_t j(0); j < this->_bufferLength; ++j)
        {
            this->ring[i].emplace_back(T(0));
        }
    }
    this->readIndex = 0;
    this->writeIndex = 0;
    this->processingIndex = 0;
}

template <typename T>
int_fast32_t RingBuffer<T>::size() const
{
    return this->_totalRingSampleLength;
}

template <typename T>
int_fast32_t RingBuffer<T>::total_size() const
{
    return this->_totalRingSampleLength * bytesPerSample;
}

template <typename T>
int_fast8_t RingBuffer<T>::ring_length() const
{
    return this->_ringLength;
}

template <typename T>
int_fast32_t RingBuffer<T>::buffer_length() const
{
    return this->_bufferLength;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_per_sample() const
{
    return bytesPerSample;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_per_buffer() const
{
    return this->_bytesPerBuffer;
}

template <typename T>
void RingBuffer<T>::fill(T value)
{
    for (int_fast32_t i(0); i < this->_bufferLength; ++i)
    {
        this->ring[0][i] = value;
    }
    for (int_fast32_t i(1); i < this->_ringLength; ++i)
    {
        std::copy(
                this->ring[0].begin(),
                this->ring[0].end(),
                this->ring[i].begin()
            );
    }
}

template <typename T>
void RingBuffer<T>::reset()
{
    this->_buffered = 0;
    this->_samplesUnread = this->_bufferLength;
    this->_samplesUnwritten = this->_bufferLength;
    this->_samplesProcessed = 0;
    this->_samplesWritten = 0;
    this->readIndex = 0;
    this->writeIndex = 0;
    this->processingIndex = 0;
}

template <typename T>
bool RingBuffer<T>::is_writable() const
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    // is (available() > 0) in this function redundant?
    return (this->readIndex != this->writeIndex) && (available() > 0);
}

template <typename T>
inline int_fast8_t RingBuffer<T>::get_ring_index(std::vector<T>* bufferPtr)
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    for (uint8_t i(0); i < this->_ringLength; ++i)
    {
        if (&(this->ring[i]) == bufferPtr)
        {
            return i;
        }
    }

    #ifdef _DEBUG
    throw std::out_of_range("Buffer not found");
    #endif

    return BUFFER_ADDR_NOT_FOUND;
}

template <typename T>
inline int_fast8_t RingBuffer<T>::get_ring_index(uint8_t* bufferPtr)
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    for (uint8_t i(0); i < this->_ringLength; ++i)
    {
        if (reinterpret_cast<uint8_t*>(&(this->ring[i][0])) == bufferPtr)
        {
            return i;
        }
    }

    #ifdef _DEBUG
    throw std::out_of_range("Buffer not found");
    #endif

    return BUFFER_ADDR_NOT_FOUND;
}

template <typename T>
int_fast32_t RingBuffer<T>::buffered() const
{
    return this->_buffered;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_buffered() const
{
    return this->_buffered * bytesPerSample;
}

template <typename T>
int_fast32_t RingBuffer<T>::available() const
{
    return this->_totalWritableLength - this->_buffered;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_available() const
{
    return (
            (this->_totalWritableLength - this->_buffered)
            * bytesPerSample
        );
}

template <typename T>
int_fast32_t RingBuffer<T>::processed() const
{
    return this->_samplesProcessed;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_processed() const
{
    return this->_samplesProcessed * bytesPerSample;
}

template <typename T>
int_fast32_t RingBuffer<T>::unprocessed() const
{
    return this->_buffered - this->_samplesProcessed;
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_unprocessed() const
{
    return (
            (this->_buffered - this->_samplesProcessed)
            * bytesPerSample
        );
}

template <typename T>
int_fast32_t RingBuffer<T>::unread() const
{
    return (buffered() ? this->_samplesUnread : 0);
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_unread() const
{
    return (
            (buffered() ? this->_samplesUnread : 0)
            * bytesPerSample
        );
}

template <typename T>
int_fast32_t RingBuffer<T>::unwritten() const
{
    return (available() ? this->_samplesUnwritten : 0);
}

template <typename T>
int_fast32_t RingBuffer<T>::bytes_unwritten() const
{
    return (
            (available() ? this->_samplesUnwritten : 0)
            * bytesPerSample
        );
}

template <typename T>
int_fast32_t RingBuffer<T>::buffers_buffered() const
{
    return (this->_buffered / this->_bufferLength);
}

template <typename T>
int_fast32_t RingBuffer<T>::buffers_available() const
{
    return (available() / this->_bufferLength);
}

template <typename T>
int_fast32_t RingBuffer<T>::buffers_processed() const
{
    return (this->_samplesProcessed / this->_bufferLength);
}

template <typename T>
inline std::vector<T> RingBuffer<T>::_read()
{
    return this->ring[this->readIndex];
}

template <typename T>
std::vector<T> RingBuffer<T>::read()
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    if (!buffers_buffered()) throw READ_BUFFER_UNDERRUN;
    #endif

    std::vector<T> output(_read());
    rotate_read_buffer();
    return output;
}

template <typename T>
void RingBuffer<T>::read_samples(T* data, int_fast32_t length)
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    if (!length) throw VALUE_MUST_BE_NONZERO;
    if (length > this->_samplesUnread)
    {
        throw std::out_of_range("Must be <= samples unread");
    }
    #endif

    std::copy(get_read_sample(), get_read_sample() + length, data);
    report_read_samples(length);
}

template <typename T>
void RingBuffer<T>::read_bytes(uint8_t* data, int_fast32_t numBytes)
{
    #ifdef _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    read_samples(reinterpret_cast<T*>(data), numBytes / bytesPerSample);
}

template <typename T>
inline std::vector<T>* RingBuffer<T>::get_read_buffer()
{
    return &(this->ring[this->readIndex]);
}

template <typename T>
inline T* RingBuffer<T>::get_read_buffer_sample()
{
    return &(this->ring[this->readIndex][0]);
}

template <typename T>
inline uint8_t* RingBuffer<T>::get_read_buffer_byte()
{
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->readIndex][0])
        );
}

template <typename T>
inline T* RingBuffer<T>::get_read_sample()
{
    return &(this->ring[this->readIndex][
            this->_bufferLength
            - this->_samplesUnread
        ]);
}

template <typename T>
inline uint8_t* RingBuffer<T>::get_read_byte()
{
    return reinterpret_cast<uint8_t*>(get_read_sample());
}

template <typename T>
inline void RingBuffer<T>::rotate_read_index()
{
    ++this->readIndex %= this->_ringLength;
}

template <typename T>
void RingBuffer<T>::rotate_read_buffer()
{
    rotate_read_index();
    this->_samplesUnread = this->_bufferLength;
    this->_buffered -= this->_bufferLength;
    this->_samplesProcessed -= this->_bufferLength;
    this->_buffered = (this->_buffered < 0) ? 0 : this->_buffered;
    this->_samplesProcessed = (
            (this->_samplesProcessed < 0)
            ? 0 : this->_samplesProcessed
        );
}

template <typename T>
void RingBuffer<T>::rotate_partial_read(int_fast32_t length)
{
    #ifdef _DEBUG
    if (length > this->_bufferLength)
    {
        throw std::out_of_range("Length must be <= buffer length");
    }
    #endif

    rotate_read_index();
    this->_buffered -= length;
    this->_samplesUnread = this->_bufferLength;
    this->_samplesProcessed -= length;

    /* Only clip negative integers if Debug is enabled */
    #ifdef _DEBUG
    this->_buffered = (this->_buffered < 0) ? 0 : this->_buffered;
    this->_samplesProcessed = (
            (this->_samplesProcessed < 0)
            ? 0 : this->_samplesProcessed
        );
    #endif
}

template <typename T>
inline void RingBuffer<T>::report_read_samples(int_fast32_t length)
{
    #ifdef _DEBUG
    if (length > this->_samplesUnread)
    {
        std::cerr << "Length must be <= unread samples\n";
        std::cerr << "Samples unread: " << this->_samplesUnread << '\n';
        throw std::out_of_range("Length must be <= unread samples");
    }
    #endif

    this->_samplesUnread -= length;
    if (!this->_samplesUnread)
    {
        rotate_read_buffer();
    }
}

template <typename T>
inline void RingBuffer<T>::report_read_bytes(int_fast32_t numBytes)
{
    #ifdef _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_read_samples(numBytes / bytesPerSample);
}

template <typename T>
int_fast32_t RingBuffer<T>::write(T data, bool force)
{
    #ifdef _DEBUG
    if (!_size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    // This could be a problem with all indeces starting at 0
    if (!is_writable() && !force) return 0;

    this->ring[this->writeIndex][this->_samplesWritten] = data;
    ++this->_samplesWritten;

    #ifdef _DEBUG
    if (--this->_samplesUnwritten <= 0)
    {
        rotate_write_buffer(force);
    }
    #endif

    ++this->_buffered;
    return 1;
}

template <typename T>
int_fast32_t RingBuffer<T>::write(std::vector<T> data, bool force)
{
    int_fast32_t written(0), remaining(static_cast<int_fast32_t>(data.size()));
    int_fast8_t index(this->_ringLength);
    while ((remaining > 0) && (is_writable() || force) && (index-- > 0))
    {
        if (remaining > this->_samplesUnwritten)
        {
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->_samplesUnwritten,
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            written += this->_samplesUnwritten;
            remaining -= this->_samplesUnwritten;
            rotate_write_buffer(force);
        }
        else
        {
            std::copy(
                    data.begin() + written,
                    data.end(),
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            this->_samplesWritten += remaining;
            this->_samplesUnwritten -= remaining;
            written += remaining;
            remaining = 0;
            if (this->_samplesUnwritten <= 0)
            {
                rotate_write_buffer(force);
            }
        }
    }
    return written;
}

template <typename T>
int_fast32_t RingBuffer<T>::write_samples(T* data, int_fast32_t length, bool force)
{
    return write(std::vector<T>(data, data + length), force);
}

template <typename T>
int_fast32_t RingBuffer<T>::write_bytes(
        uint8_t* data,
        int_fast32_t numBytes,
        bool force
    )
{
    return write(std::vector<T>(
            reinterpret_cast<T*>(data),
            reinterpret_cast<T*>(data + numBytes)
        ), force) * bytesPerSample;
}

template <typename T>
inline std::vector<T>* RingBuffer<T>::get_write_buffer()
{
    return &(this->ring[this->writeIndex]);
}

template <typename T>
inline T* RingBuffer<T>::get_write_buffer_sample()
{
    return &(this->ring[this->writeIndex][0]);
}

template <typename T>
inline uint8_t* RingBuffer<T>::get_write_buffer_byte()
{
    return reinterpret_cast<uint8_t*>(get_write_buffer_sample());
}

template <typename T>
T* RingBuffer<T>::get_write_sample()
{
    return &(this->ring[this->writeIndex][
            this->_bufferLength
            - this->_samplesUnwritten
        ]);
}

template <typename T>
uint8_t* RingBuffer<T>::get_write_byte()
{
    return reinterpret_cast<uint8_t*>(get_write_sample());
}

template <typename T>
inline void RingBuffer<T>::rotate_write_index()
{
    ++this->writeIndex %= this->_ringLength;
}

template <typename T>
void RingBuffer<T>::rotate_write_buffer(bool force)
{
    rotate_write_index();
    this->_samplesWritten = 0;
    this->_samplesUnwritten = this->_bufferLength;
    this->_buffered += this->_bufferLength;
    if (force && !is_writable())
    {
        rotate_read_buffer();
    }
}

template <typename T>
void RingBuffer<T>::rotate_partial_write(int_fast32_t length, bool force)
{
    #ifdef _DEBUG
    if (length > this->_bufferLength)
    {
        throw std::out_of_range("Length must be <= buffer length");
    }
    #endif

    rotate_write_index();
    this->_samplesWritten = 0;
    this->_samplesUnwritten = this->_bufferLength;
    this->_buffered += length;
    this->_buffered = (
            (this->_buffered > this->_totalWritableLength)
            ? this->_totalWritableLength : this->_buffered
        );
    if (force && !is_writable())
    {
        rotate_read_buffer();
    }
}

template <typename T>
inline void RingBuffer<T>::report_written_samples(int_fast32_t length)
{
    #ifdef _DEBUG
    if (length > this->_samplesUnwritten)
    {
        std::cerr << "Length must be <= unwritten samples\n";
        std::cerr << "Samples unwritten: " << this->_samplesUnwritten << '\n';
        throw std::out_of_range("Length must be <= unwritten samples");
    }
    #endif

    this->_samplesWritten += length;
    this->_samplesUnwritten -= length;
    if (!this->_samplesUnwritten)
    {
        rotate_write_buffer();
    }
}

template <typename T>
inline void RingBuffer<T>::report_written_bytes(int_fast32_t numBytes)
{
    #ifdef _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_written_samples(numBytes / bytesPerSample);
}

template <typename T>
std::vector<T>* RingBuffer<T>::get_processing_buffer()
{
    return &(this->ring[this->processingIndex]);
}

template <typename T>
inline T* RingBuffer<T>::get_processing_buffer_sample()
{
    return &(this->ring[this->processingIndex][0]);
}

template <typename T>
uint8_t* RingBuffer<T>::get_processing_buffer_byte()
{
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->processingIndex][0])
        );
}

template <typename T>
T* RingBuffer<T>::get_processing_sample()
{
    return &(this->ring[this->processingIndex][this->_samplesProcessed]);
}

template <typename T>
uint8_t* RingBuffer<T>::get_processing_byte()
{
    return reinterpret_cast<uint8_t*>(get_processing_sample());
}

template <typename T>
inline void RingBuffer<T>::rotate_processing_index()
{
    ++this->processingIndex %= this->_ringLength;
}

template <typename T>
void RingBuffer<T>::rotate_processing_buffer()
{
    rotate_processing_index();
    this->_samplesProcessed += this->_bufferLength;
}

template <typename T>
void RingBuffer<T>::rotate_partial_processing(int_fast32_t length)
{
    #ifdef _DEBUG
    if (length > this->_bufferLength)
    {
        throw std::out_of_range("Length must be <= buffer length");
    }
    #endif

    rotate_processing_index();
    this->_samplesProcessed += length;
    this->_samplesProcessed = (
        (this->_samplesProcessed > this->_totalWritableLength)
        ? this->_totalWritableLength : this->_samplesProcessed
    );
}

template <typename T>
inline void RingBuffer<T>::report_processed_samples(int_fast32_t length)
{
    #ifdef _DEBUG
    if (length > unprocessed())
    {
        throw std::out_of_range("Length must be <= unprocessed samples");
    }
    #endif

    int_fast32_t unprocessedSamples = (
            this->_bufferLength - this->_samplesProcessed
        );
    while (length)
    {
        unprocessedSamples = (
                (length < unprocessedSamples)
                ? length
                : unprocessedSamples
            );
        this->_samplesProcessed += unprocessedSamples;
        if (this->_samplesProcessed == this->_bufferLength)
        {
            rotate_processing_buffer();
        }
        length -= unprocessedSamples;
    }
}

template <typename T>
inline void RingBuffer<T>::report_processed_bytes(int_fast32_t numBytes)
{
    #ifdef _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_processed_samples(numBytes / bytesPerSample);
}

template class Buffer::RingBuffer<int8_t>;
template class Buffer::RingBuffer<uint8_t>;
template class Buffer::RingBuffer<int16_t>;
template class Buffer::RingBuffer<uint16_t>;
template class Buffer::RingBuffer<int32_t>;
template class Buffer::RingBuffer<uint32_t>;
template class Buffer::RingBuffer<int64_t>;
template class Buffer::RingBuffer<uint64_t>;

template class Buffer::RingBuffer<int>;

template class Buffer::RingBuffer<float>;
template class Buffer::RingBuffer<double>;
template class Buffer::RingBuffer<long double>;

template class Buffer::RingBuffer<char>;
template class Buffer::RingBuffer<wchar_t>;
template class Buffer::RingBuffer<char16_t>;
template class Buffer::RingBuffer<char32_t>;

