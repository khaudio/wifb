#include "ringbuffer.h"

using namespace Buffer;

template <typename T, typename I>
Base<T, I>::Base() :
_firstWritten(false),
_ringLength(0),
_buffered(0),
_samplesWritten(0),
_samplesUnwritten(0),
_samplesProcessed(0),
_samplesUnread(0),
_bufferLength(0),
_bytesPerBuffer(0),
_totalWritableLength(0),
_totalRingSampleLength(0)
{
}

template <typename T, typename I>
Base<T, I>::Base(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    set_size(bufferSize, ringSize);
}

template <typename T, typename I>
Base<T, I>::Base(const Base& obj)
{
    /* make a simple copy constructor; i.e., assign values,
    not set_size() because running copy const when pushing
    into a vector is triggering error checking before size
    can be properly set.  default constructor is being used
    to initialize but not set anything */
    
    set_size(obj.buffer_length(), obj.ring_length());
    
    int_fast8_t readidx = obj.readIndex;
    this->readIndex = readidx;
    int_fast8_t writeidx = obj.writeIndex;
    this->writeIndex = writeidx;
    int_fast8_t procidx = obj.processingIndex;
    this->processingIndex = procidx;
    
    this->_firstWritten = obj._firstWritten;
    this->_buffered = obj._buffered;
    this->_samplesWritten = obj._samplesWritten;
    this->_samplesUnwritten = obj._samplesUnwritten;
    this->_samplesProcessed = obj._samplesProcessed;
    this->_samplesUnread = obj._samplesUnread;
}

template <typename T, typename I>
Base<T, I>::~Base()
{
}

template <typename T, typename I>
inline bool Base<T, I>::size_is_set() const
{
    return ((this->_bufferLength > 0) && (this->_ringLength > 0));
}

template <typename T, typename I>
void Base<T, I>::set_size(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    #if _DEBUG
    /* There must be multiple buffers to rotate */
    if (ringSize < 2) throw RING_SIZE_TOO_SHORT;

    /* Assert that total ring size in bytes
    is less than datatype max */
    if (bufferSize > (
            std::numeric_limits<int_fast32_t>::max()
            / static_cast<int_fast32_t>(ringSize)
            / bytesPerSample
        ))
    {
        throw BUFFER_LENGTH_TOO_LONG;
    }

    /* Buffer length must be an even number */
    if (bufferSize % 2) throw UNEVEN_BUFFER_LENGTH;
    
    /* Buffer length must be divisible by ring length */
    if (bufferSize % ringSize) throw NON_MULTIPLE_BUFFER_LENGTH;
    #endif

    this->_firstWritten = false;
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
    this->readIndex = this->_ringLength - 1;
    this->writeIndex = 0;
    this->processingIndex = 0;
}

template <typename T, typename I>
int_fast32_t Base<T, I>::size() const
{
    return this->_totalRingSampleLength;
}

template <typename T, typename I>
int_fast32_t Base<T, I>::total_size() const
{
    return this->_totalRingSampleLength * bytesPerSample;
}

template <typename T, typename I>
int_fast8_t Base<T, I>::ring_length() const
{
    return this->_ringLength;
}

template <typename T, typename I>
int_fast32_t Base<T, I>::buffer_length() const
{
    return this->_bufferLength;
}

template <typename T, typename I>
int_fast32_t Base<T, I>::bytes_per_sample() const
{
    return bytesPerSample;
}

template <typename T, typename I>
int_fast32_t Base<T, I>::bytes_per_buffer() const
{
    return this->_bytesPerBuffer;
}

template <typename T, typename I>
void Base<T, I>::reset()
{
    this->_firstWritten = false;
    this->_buffered = 0;
    this->_samplesUnread = this->_bufferLength;
    this->_samplesUnwritten = this->_bufferLength;
    this->_samplesProcessed = 0;
    this->_samplesWritten = 0;
    this->readIndex = this->_ringLength - 1;
    this->writeIndex = 0;
    this->processingIndex = 0;
}

template <typename T, typename I>
bool Base<T, I>::is_writable() const
{
    #if _DEBUG
    if (!size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    // is (available() > 0) in this function redundant?
    
    return (this->readIndex != this->writeIndex) && (available() > 0);
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::buffered() const
{
    return this->_buffered;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_buffered() const
{
    return buffered() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::available() const
{
    return this->_totalWritableLength - buffered();
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_available() const
{
    return available() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::processed() const
{
    return this->_samplesProcessed;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_processed() const
{
    return processed() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::unprocessed() const
{
    return buffered() - this->_samplesProcessed;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_unprocessed() const
{
    return unprocessed() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::unread() const
{
    return (buffered() ? this->_samplesUnread : 0);
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_unread() const
{
    return unread() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::unwritten() const
{
    return (available() ? this->_samplesUnwritten : 0);
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::bytes_unwritten() const
{
    return unwritten() * bytesPerSample;
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::buffers_buffered() const
{
    return (buffered() / this->_bufferLength);
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::buffers_available() const
{
    return (available() / this->_bufferLength);
}

template <typename T, typename I>
inline int_fast32_t Base<T, I>::buffers_processed() const
{
    return (processed() / this->_bufferLength);
}

template <typename T, typename I>
inline void Base<T, I>::rotate_read_index()
{
    ++this->readIndex;
    this->readIndex = this->readIndex % this->_ringLength;
}

template <typename T, typename I>
void Base<T, I>::rotate_read_buffer()
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

template <typename T, typename I>
void Base<T, I>::rotate_partial_read(int_fast32_t length)
{
    #if _DEBUG
    if (length > this->_bufferLength)
    {
        throw std::out_of_range("Length must be <= buffer length");
    }
    #endif

    rotate_read_index();
    this->_buffered -= length;
    this->_samplesUnread = this->_bufferLength;
    this->_samplesProcessed -= length;
    this->_buffered = (this->_buffered < 0) ? 0 : this->_buffered;
    this->_samplesProcessed = (
            (this->_samplesProcessed < 0)
            ? 0 : this->_samplesProcessed
        );
}

template <typename T, typename I>
inline void Base<T, I>::report_read_samples(int_fast32_t length)
{
    #if _DEBUG
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

template <typename T, typename I>
inline void Base<T, I>::report_read_bytes(int_fast32_t numBytes)
{
    #if _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_read_samples(numBytes / bytesPerSample);
}

template <typename T, typename I>
inline void Base<T, I>::rotate_write_index()
{
    ++this->writeIndex;
    this->writeIndex = this->writeIndex % this->_ringLength;

    #if RINGBUFF_AUTO_FIRST_ROTATE
    if (!this->_firstWritten)
    {
        this->_firstWritten = true;
        rotate_read_index();
    }
    #endif
}

template <typename T, typename I>
void Base<T, I>::rotate_write_buffer(bool force)
{
    rotate_write_index();
    this->_samplesWritten = 0;
    this->_samplesUnwritten = this->_bufferLength;
    this->_buffered += this->_bufferLength;
    /* if (force && !is_writable())
    {
        rotate_read_index();
    } */
}

template <typename T, typename I>
void Base<T, I>::rotate_partial_write(int_fast32_t length, bool force)
{
    #if _DEBUG
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
    /* if (force && !is_writable())
    {
        rotate_read_index();
    } */
}

template <typename T, typename I>
inline void Base<T, I>::report_written_samples(int_fast32_t length)
{
    #if _DEBUG
    if (length > this->_samplesUnwritten)
    {
        std::cerr << "Length must be <= unwritten samples\n";
        std::cerr << "Samples unwritten: " << this->_samplesUnwritten << '\n';
        throw std::out_of_range("Length must be <= unwritten samples");
    }
    else if (length <= 0)
    {
        throw std::out_of_range("Length must be > 0");
    }
    #endif

    this->_samplesWritten += length;
    this->_samplesUnwritten -= length;
    if (!this->_samplesUnwritten)
    {
        rotate_write_buffer();
    }
}

template <typename T, typename I>
inline void Base<T, I>::report_written_bytes(int_fast32_t numBytes)
{
    #if _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_written_samples(numBytes / bytesPerSample);
}

template <typename T, typename I>
inline void Base<T, I>::rotate_processing_index()
{
    ++this->processingIndex;
    this->processingIndex = this->processingIndex % this->_ringLength;
}

template <typename T, typename I>
void Base<T, I>::rotate_processing_buffer()
{
    rotate_processing_index();
    this->_samplesProcessed += this->_bufferLength;
}

template <typename T, typename I>
void Base<T, I>::rotate_partial_processing(int_fast32_t length)
{
    #if _DEBUG
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

template <typename T, typename I>
inline void Base<T, I>::report_processed_samples(int_fast32_t length)
{
    #if _DEBUG
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

template <typename T, typename I>
inline void Base<T, I>::report_processed_bytes(int_fast32_t numBytes)
{
    #if _DEBUG
    if (numBytes % bytesPerSample) throw NON_MULTIPLE_BYTE_COUNT;
    #endif

    report_processed_samples(numBytes / bytesPerSample);
}

template <typename T, typename I>
RingBuffer<T, I>::RingBuffer() :
Base<T, I>()
{
}

template <typename T, typename I>
RingBuffer<T, I>::RingBuffer(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    set_size(bufferSize, ringSize);
}

template <typename T, typename I>
RingBuffer<T, I>::RingBuffer(const RingBuffer& obj) :
Base<T, I>(obj)
{
    set_size(obj.buffer_length(), obj.ring_length());
    
    std::copy(
            obj.ring.begin(),
            obj.ring.end(),
            this->ring.begin()
        );

    // for (int_fast8_t i(0); i < this->_ringLength; ++i)
    // {
    //     #if _DEBUG
    //     std::copy(
    //             obj.ring.at(i).begin(),
    //             obj.ring.at(i).end(),
    //             this->ring.at(i).begin()
    //         );
    //     #else
    //     std::copy(
    //             obj.ring[i].begin(),
    //             obj.ring[i].end(),
    //             this->ring[i].begin()
    //         );
    //     #endif
    // }
}

template <typename T, typename I>
RingBuffer<T, I>::~RingBuffer()
{
}

template <typename T, typename I>
void RingBuffer<T, I>::set_size(int_fast32_t bufferSize, int_fast8_t ringSize)
{
    Base<T, I>::set_size(bufferSize, ringSize);
    this->ring = std::vector<std::vector<T>>(this->_ringLength);
    for (int_fast8_t i(0); i < this->_ringLength; ++i)
    {
        #if _DEBUG
        this->ring.at(i).reserve(this->_bufferLength);
        #else
        this->ring[i].reserve(this->_bufferLength);
        #endif
        for (int_fast32_t j(0); j < this->_bufferLength; ++j)
        {
            #if _DEBUG
            this->ring.at(i).emplace_back(0);
            #else
            this->ring[i].emplace_back(0);
            #endif
        }
    }
}

template <typename T, typename I>
void RingBuffer<T, I>::fill(T value)
{
    // for (int_fast32_t i(1); i < this->_ringLength; ++i)
    // {
    //     for (int_fast32_t j(0); j < this->_bufferLength; ++j)
    //     {
    //         #if _DEBUG
    //         this->ring.at(i).at(j) = value;
    //         #else
    //         this->ring[i][j] = value;
    //         #endif
    //     }
    // }

    for (int_fast32_t i(0); i < this->_bufferLength; ++i)
    {
        #if _DEBUG
        this->ring.at(0).at(i) = value;
        #else
        this->ring[0][i] = value;
        #endif
    }
    
    for (int_fast32_t i(1); i < this->_ringLength; ++i)
    {
        #if _DEBUG
        std::copy(
                this->ring.at(0).begin(),
                this->ring.at(0).end(),
                this->ring.at(i).begin()
            );
        #else
        std::copy(
                this->ring[0].begin(),
                this->ring[0].end(),
                this->ring[i].begin()
            );
        #endif
    }
}

template <typename T, typename I>
inline int_fast8_t RingBuffer<T, I>::get_ring_index(std::vector<T>* bufferPtr)
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    for (int_fast8_t i(0); i < this->_ringLength; ++i)
    {
        #if _DEBUG
        if (&(this->ring.at(i)) == bufferPtr)
        #else
        if (&(this->ring[i]) == bufferPtr)
        #endif
        {
            return i;
        }
    }

    #if _DEBUG
    throw std::out_of_range("Buffer not found");
    #else
    return BUFFER_ADDR_NOT_FOUND;
    #endif
}

template <typename T, typename I>
inline int_fast8_t RingBuffer<T, I>::get_ring_index(uint8_t* bufferPtr)
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    for (uint8_t i(0); i < this->_ringLength; ++i)
    {
        if (reinterpret_cast<uint8_t*>(&(this->ring[i][0])) == bufferPtr)
        {
            return i;
        }
    }

    #if _DEBUG
    throw std::out_of_range("Buffer not found");
    #else
    return BUFFER_ADDR_NOT_FOUND;
    #endif
}

template <typename T, typename I>
inline std::vector<T> RingBuffer<T, I>::_read()
{
    #if _DEBUG
    return this->ring.at(this->readIndex);
    #else
    return this->ring[this->readIndex];
    #endif
}

template <typename T, typename I>
std::vector<T> RingBuffer<T, I>::read()
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    if (!Base<T, I>::buffers_buffered()) throw READ_BUFFER_UNDERRUN;
    #endif

    std::vector<T> output(_read());
    Base<T, I>::rotate_read_buffer();
    return output;
}

template <typename T, typename I>
void RingBuffer<T, I>::read_samples(T* data, int_fast32_t length)
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    if (!length) throw VALUE_MUST_BE_NONZERO;
    if (length > this->_samplesUnread)
    {
        throw std::out_of_range("Must be <= samples unread");
    }
    #endif

    std::copy(get_read_sample(), get_read_sample() + length, data);
    Base<T, I>::report_read_samples(length);
}

template <typename T, typename I>
void RingBuffer<T, I>::read_bytes(uint8_t* data, int_fast32_t numBytes)
{
    #if _DEBUG
    if (numBytes % Base<T, I>::bytesPerSample)
    {
        throw NON_MULTIPLE_BYTE_COUNT;
    }
    #endif

    read_samples(
            reinterpret_cast<T*>(data),
            numBytes / Base<T, I>::bytesPerSample
        );
}

template <typename T, typename I>
inline std::vector<T>* RingBuffer<T, I>::get_read_buffer()
{
    #if _DEBUG
    return &(this->ring.at(this->readIndex));
    #else
    return &(this->ring[this->readIndex]);
    #endif
}

template <typename T, typename I>
inline T* RingBuffer<T, I>::get_read_buffer_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->readIndex).at(0));
    #else
    return &(this->ring[this->readIndex][0]);
    #endif
}

template <typename T, typename I>
inline uint8_t* RingBuffer<T, I>::get_read_buffer_byte()
{
    #if _DEBUG
    return reinterpret_cast<uint8_t*>(
            &(this->ring.at(this->readIndex).at(0))
        );
    #else
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->readIndex][0])
        );
    #endif
}

template <typename T, typename I>
inline T* RingBuffer<T, I>::get_read_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->readIndex).at(
            this->_bufferLength - this->_samplesUnread
        ));
    #else
    return &(this->ring[this->readIndex][
            this->_bufferLength - this->_samplesUnread
        ]);
    #endif
}

template <typename T, typename I>
inline uint8_t* RingBuffer<T, I>::get_read_byte()
{
    return reinterpret_cast<uint8_t*>(get_read_sample());
}

template <typename T, typename I>
int_fast32_t RingBuffer<T, I>::write(T data, bool force)
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif

    if (!Base<T, I>::is_writable() && !force) return 0;

    this->ring[this->writeIndex][this->_samplesWritten] = data;
    ++this->_samplesWritten;

    #if _DEBUG
    if (--this->_samplesUnwritten <= 0)
    {
        Base<T, I>::rotate_write_buffer(force);
    }
    #endif

    ++this->_buffered;
    return 1;
}

template <typename T, typename I>
int_fast32_t RingBuffer<T, I>::write(std::vector<T> data, bool force)
{
    #if _DEBUG
    if (!Base<T, I>::size_is_set()) throw BUFFER_NOT_INITIALIZED;
    #endif
    
    int_fast32_t written(0), remaining(static_cast<int_fast32_t>(data.size()));
    int_fast8_t index(this->_ringLength);

    while (
            (remaining > 0)
            && (Base<T, I>::is_writable() || force)
            && (index-- > 0)
        )
    {
        if (remaining > this->_samplesUnwritten)
        {
            #if _DEBUG
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->_samplesUnwritten,
                    this->ring.at(this->writeIndex).begin() + this->_samplesWritten
                );
            #else
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->_samplesUnwritten,
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            #endif
            written += this->_samplesUnwritten;
            remaining -= this->_samplesUnwritten;
            Base<T, I>::rotate_write_buffer(force);
        }
        else
        {
            #if _DEBUG
            std::copy(
                    data.begin() + written,
                    data.end(),
                    this->ring.at(this->writeIndex).begin() + this->_samplesWritten
                );
            #else
            std::copy(
                    data.begin() + written,
                    data.end(),
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            #endif
            this->_samplesWritten += remaining;
            this->_samplesUnwritten -= remaining;
            written += remaining;
            remaining = 0;
            if (this->_samplesUnwritten <= 0)
            {
                Base<T, I>::rotate_write_buffer(force);
            }
        }
    }
    return written;
}

template <typename T, typename I>
int_fast32_t RingBuffer<T, I>::write_samples(T* data, int_fast32_t length, bool force)
{
    return write(std::vector<T>(data, data + length), force);
}

template <typename T, typename I>
int_fast32_t RingBuffer<T, I>::write_bytes(
        uint8_t* data,
        int_fast32_t numBytes,
        bool force
    )
{
    return write(std::vector<T>(
            reinterpret_cast<T*>(data),
            reinterpret_cast<T*>(data + numBytes)
        ), force) * Base<T, I>::bytesPerSample;
}

template <typename T, typename I>
inline std::vector<T>* RingBuffer<T, I>::get_write_buffer()
{
    #if _DEBUG
    return &(this->ring.at(this->writeIndex));
    #else
    return &(this->ring[this->writeIndex]);
    #endif
}

template <typename T, typename I>
inline T* RingBuffer<T, I>::get_write_buffer_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->writeIndex).at(0));
    #else
    return &(this->ring[this->writeIndex][0]);
    #endif
}

template <typename T, typename I>
inline uint8_t* RingBuffer<T, I>::get_write_buffer_byte()
{
    return reinterpret_cast<uint8_t*>(get_write_buffer_sample());
}

template <typename T, typename I>
T* RingBuffer<T, I>::get_write_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->writeIndex).at(
            this->_bufferLength - this->_samplesUnwritten
        ));
    #else
    return &(this->ring[this->writeIndex][
            this->_bufferLength - this->_samplesUnwritten
        ]);
    #endif
}

template <typename T, typename I>
uint8_t* RingBuffer<T, I>::get_write_byte()
{
    return reinterpret_cast<uint8_t*>(get_write_sample());
}

template <typename T, typename I>
std::vector<T>* RingBuffer<T, I>::get_processing_buffer()
{
    #if _DEBUG
    return &(this->ring.at(this->processingIndex));
    #else
    return &(this->ring[this->processingIndex]);
    #endif
}

template <typename T, typename I>
inline T* RingBuffer<T, I>::get_processing_buffer_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->processingIndex).at(0));
    #else
    return &(this->ring[this->processingIndex][0]);
    #endif
}

template <typename T, typename I>
uint8_t* RingBuffer<T, I>::get_processing_buffer_byte()
{
    #if _DEBUG
    return reinterpret_cast<uint8_t*>(
            &(this->ring.at(this->processingIndex).at(0))
        );
    #else
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->processingIndex][0])
        );
    #endif
}

template <typename T, typename I>
T* RingBuffer<T, I>::get_processing_sample()
{
    #if _DEBUG
    return &(this->ring.at(this->processingIndex).at(this->_samplesProcessed));
    #else
    return &(this->ring[this->processingIndex][this->_samplesProcessed]);
    #endif
}

template <typename T, typename I>
uint8_t* RingBuffer<T, I>::get_processing_byte()
{
    return reinterpret_cast<uint8_t*>(get_processing_sample());
}

template <typename T>
NonAtomicRingBuffer<T>::NonAtomicRingBuffer() :
RingBuffer<T, int_fast8_t>()
{
}

template <typename T>
NonAtomicRingBuffer<T>::NonAtomicRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize
    ) :
RingBuffer<T, int_fast8_t>(bufferSize, ringSize)
{
}

template <typename T>
NonAtomicRingBuffer<T>::NonAtomicRingBuffer(
        const NonAtomicRingBuffer& obj
    ) :
RingBuffer<T, int_fast8_t>(obj)
{
}

template <typename T>
NonAtomicRingBuffer<T>::~NonAtomicRingBuffer()
{
}

template <typename T>
AtomicRingBuffer<T>::AtomicRingBuffer() :
RingBuffer<T, std::atomic_int_fast8_t>()
{
}

template <typename T>
AtomicRingBuffer<T>::AtomicRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize
    ) :
RingBuffer<T, std::atomic_int_fast8_t>(bufferSize, ringSize)
{
}

template <typename T>
AtomicRingBuffer<T>::AtomicRingBuffer(
        const AtomicRingBuffer& obj
    ) :
RingBuffer<T, std::atomic_int_fast8_t>(obj)
{
}

template <typename T>
AtomicRingBuffer<T>::~AtomicRingBuffer()
{
}

template <typename T>
AtomicMultiReadRingBuffer<T>::AtomicMultiReadRingBuffer() :
AtomicRingBuffer<T>()
{
}

template <typename T>
AtomicMultiReadRingBuffer<T>::AtomicMultiReadRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize
    ) :
AtomicRingBuffer<T>(bufferSize, ringSize)
{
}

template <typename T>
AtomicMultiReadRingBuffer<T>::AtomicMultiReadRingBuffer(
        const AtomicMultiReadRingBuffer& obj
    ) :
AtomicRingBuffer<T>(obj)
{
    int_fast8_t readers = obj._numReaders;
    this->_numReaders = readers;
    int_fast8_t counter = obj._readCounter;
    this->_readCounter = counter;
}

template <typename T>
AtomicMultiReadRingBuffer<T>::~AtomicMultiReadRingBuffer()
{
}

template <typename T>
inline bool AtomicMultiReadRingBuffer<T>::_increment_read_counter()
{
    this->_readCounter = ++this->_readCounter % this->_numReaders;
    return !this->_readCounter;
}

template <typename T>
inline void AtomicMultiReadRingBuffer<T>::set_num_readers(int_fast8_t numReaders)
{
    #if _DEBUG
    if (
            (numReaders <= 0)
            || (numReaders > std::numeric_limits<int_fast8_t>::max())
        )
    {
        throw std::out_of_range(
                "Number of readers must be 0 < num readers < datatype max"
            );
    }
    #endif
    this->_numReaders = numReaders;
}

template <typename T>
inline int_fast8_t AtomicMultiReadRingBuffer<T>::num_readers() const
{
    return this->_numReaders;
}

template <typename T>
inline void AtomicMultiReadRingBuffer<T>::report_read_samples(int_fast32_t length)
{
    if (!_increment_read_counter()) return;
    AtomicRingBuffer<T>::report_read_samples(length);
}

template <typename T>
std::vector<T> AtomicMultiReadRingBuffer<T>::read()
{
    std::vector<T> output(AtomicRingBuffer<T>::_read());
    report_read_samples(this->_bufferLength);
    return output;
}

/*                      Template Instantiations                     */

/*                               Base                               */

// template class Buffer::Base<int8_t, int_fast8_t>;
// template class Buffer::Base<uint8_t, int_fast8_t>;
// template class Buffer::Base<int16_t, int_fast8_t>;
// template class Buffer::Base<uint16_t, int_fast8_t>;
// template class Buffer::Base<int32_t, int_fast8_t>;
// template class Buffer::Base<uint32_t, int_fast8_t>;

template class Buffer::Base<uint8_t, int_fast8_t>;
template class Buffer::Base<int16_t, int_fast8_t>;
template class Buffer::Base<int_fast32_t, int_fast8_t>;

// template class Buffer::Base<float, int_fast8_t>;
// template class Buffer::Base<double, int_fast8_t>;

/*                       Atomic Indices Base                        */

// template class Buffer::Base<int8_t, std::atomic_int_fast8_t>;
// template class Buffer::Base<uint8_t, std::atomic_int_fast8_t>;
// template class Buffer::Base<int16_t, std::atomic_int_fast8_t>;
// template class Buffer::Base<uint16_t, std::atomic_int_fast8_t>;
// template class Buffer::Base<int32_t, std::atomic_int_fast8_t>;
// template class Buffer::Base<uint32_t, std::atomic_int_fast8_t>;

template class Buffer::Base<uint8_t, std::atomic_int_fast8_t>;
template class Buffer::Base<int16_t, std::atomic_int_fast8_t>;
template class Buffer::Base<int_fast32_t, std::atomic_int_fast8_t>;

// template class Buffer::Base<float, std::atomic_int_fast8_t>;
// template class Buffer::Base<double, std::atomic_int_fast8_t>;

/*                           Ring Buffer                            */

// template class Buffer::RingBuffer<int8_t, int_fast8_t>;
// template class Buffer::RingBuffer<uint8_t, int_fast8_t>;
// template class Buffer::RingBuffer<int16_t, int_fast8_t>;
// template class Buffer::RingBuffer<uint16_t, int_fast8_t>;
// template class Buffer::RingBuffer<int32_t, int_fast8_t>;
// template class Buffer::RingBuffer<uint32_t, int_fast8_t>;

// template class Buffer::RingBuffer<int, int_fast8_t>;

// template class Buffer::RingBuffer<float, int_fast8_t>;
// template class Buffer::RingBuffer<double, int_fast8_t>;

// /*                    Atomic Indices Ring Buffer                    */

// template class Buffer::RingBuffer<int8_t, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<uint8_t, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<int16_t, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<uint16_t, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<int32_t, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<uint32_t, std::atomic_int_fast8_t>;

// template class Buffer::RingBuffer<int, std::atomic_int_fast8_t>;

// template class Buffer::RingBuffer<float, std::atomic_int_fast8_t>;
// template class Buffer::RingBuffer<double, std::atomic_int_fast8_t>;

// /*                           Ring Buffer                            */

// template class Buffer::NonAtomicRingBuffer<int8_t>;
// template class Buffer::NonAtomicRingBuffer<uint8_t>;
// template class Buffer::NonAtomicRingBuffer<int16_t>;
// template class Buffer::NonAtomicRingBuffer<uint16_t>;
// template class Buffer::NonAtomicRingBuffer<int32_t>;
// template class Buffer::NonAtomicRingBuffer<uint32_t>;

// template class Buffer::NonAtomicRingBuffer<int>;

// template class Buffer::NonAtomicRingBuffer<float>;
// template class Buffer::NonAtomicRingBuffer<double>;

// /*                    Atomic Indices Ring Buffer                    */

// template class Buffer::AtomicRingBuffer<int8_t>;
// template class Buffer::AtomicRingBuffer<uint8_t>;
// template class Buffer::AtomicRingBuffer<int16_t>;
// template class Buffer::AtomicRingBuffer<uint16_t>;
// template class Buffer::AtomicRingBuffer<int32_t>;
// template class Buffer::AtomicRingBuffer<uint32_t>;

// template class Buffer::AtomicRingBuffer<int>;

// template class Buffer::AtomicRingBuffer<float>;
// template class Buffer::AtomicRingBuffer<double>;

/*           Atomic Indices Multi Read Counter Ring Buffer          */

// template class Buffer::AtomicMultiReadRingBuffer<int8_t>;
// template class Buffer::AtomicMultiReadRingBuffer<uint8_t>;
// template class Buffer::AtomicMultiReadRingBuffer<int16_t>;
// template class Buffer::AtomicMultiReadRingBuffer<uint16_t>;
// template class Buffer::AtomicMultiReadRingBuffer<int32_t>;
// template class Buffer::AtomicMultiReadRingBuffer<uint32_t>;

template class Buffer::AtomicMultiReadRingBuffer<uint8_t>;
template class Buffer::AtomicMultiReadRingBuffer<int16_t>;
template class Buffer::AtomicMultiReadRingBuffer<int_fast32_t>;

// template class Buffer::AtomicMultiReadRingBuffer<float>;
// template class Buffer::AtomicMultiReadRingBuffer<double>;
