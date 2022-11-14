#include "multibuffer.h"

using namespace Buffer;

template <typename T, typename I>
MultiRingBuffer<T, I>::MultiRingBuffer() :
Base<T, I>()
{
}

template <typename T, typename I>
MultiRingBuffer<T, I>::MultiRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize,
        int_fast8_t numBuffers
    )
{
    set_size(bufferSize, ringSize, numBuffers);
}

template <typename T, typename I>
MultiRingBuffer<T, I>::MultiRingBuffer(const MultiRingBuffer& obj) :
Base<T, I>(obj),
_numBuffers(obj._numBuffers)
{
    set_size(obj.buffer_length(), obj.ring_length(), obj.num_buffers());
    for (int i(0); i < this->_numBuffers; ++i)
    {
        for (int j(0); j < obj.buffers.at(i).ring_length(); ++j)
        {
            std::copy(
                    obj.buffers.at(i).ring.at(j).begin(),
                    obj.buffers.at(i).ring.at(j).end(),
                    this->buffers.at(i).ring.at(j).begin()
                );
        }
    }
}

template <typename T, typename I>
MultiRingBuffer<T, I>::~MultiRingBuffer()
{
}

template <typename T, typename I>
bool MultiRingBuffer<T, I>::size_is_set() const
{
    for (const RingBuffer<T, I>& buff: this->buffers)
    {
        if (!buff.size_is_set())
        {
            #if _DEBUG
            std::cerr << "Error: sub buffer size not set\n";
            #endif
            
            return false;
        }
    }

    #if _DEBUG
    if (!(this->buffers.size() > 0)) std::cerr << "No sub buffers\n";
    if (!(this->_ringLength > 0)) std::cerr << "Nothing in ring\n";
    if (!(this->_bufferLength > 0)) std::cerr << "Buffers not allocated\n";
    #endif

    return (
            (this->buffers.size() > 0)
            && (this->_ringLength > 0)
            && (this->_bufferLength > 0)
        );
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::set_size(
        int_fast32_t bufferSize,
        int_fast8_t ringSize,
        int_fast8_t numBuffers
    )
{
    #if _DEBUG
    if (numBuffers < 2) throw BUFFER_COUNT_TOO_SHORT;
    #endif

    Base<T, I>::set_size(bufferSize, ringSize);
    this->_numBuffers = numBuffers;
    this->buffers.reserve(numBuffers);
    for (int i(0); i < numBuffers; ++i)
    {
        this->buffers.emplace_back(bufferSize, ringSize);
    }
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::size() const
{
    return Base<T, I>::size() * this->_numBuffers;
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::total_size() const
{
    return Base<T, I>::total_size() * this->_numBuffers;
}

template <typename T, typename I>
int_fast8_t MultiRingBuffer<T, I>::num_buffers() const
{
    return this->_numBuffers;
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::fill(T value)
{
    for (RingBuffer<T, I>& buff: this->buffers)
    {
        buff.fill(value);
    }
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::reset()
{
    for (RingBuffer<T, I>& buff: this->buffers)
    {
        buff.reset();
    }
    Base<T, I>::reset();
}

template <typename T, typename I>
auto MultiRingBuffer<T, I>::get_buffer(
        int_fast8_t bufferIndex
    )
{
    #if _DEBUG
    return this->buffers.at(bufferIndex);
    #else
    return this->buffers[bufferIndex];
    #endif
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::_update_sample_counters()
{
    #if _DEBUG
    int_fast32_t
        leastBuffered(this->buffers.at(0).buffered()),
        leastSamplesWritten(this->buffers.at(0)._samplesWritten),
        leastSamplesProcessed(this->buffers.at(0).processed()),
        leastSamplesUnread(this->buffers.at(0).unread());
    #else
    int_fast32_t
        leastBuffered(this->buffers[0].buffered()),
        leastSamplesWritten(this->buffers[0]._samplesWritten),
        leastSamplesProcessed(this->buffers[0].processed()),
        leastSamplesUnread(this->buffers[0].unread());
    #endif
    for (int i(1); i < this->_numBuffers; ++i)
    {
        #if _DEBUG
        RingBuffer<T, I>& buff = this->buffers.at(i);
        #else
        RingBuffer<T, I>& buff = this->buffers[i];
        #endif

        leastBuffered = (
                (buff._buffered < leastBuffered)
                ? buff._buffered
                : leastBuffered
            );
        leastSamplesWritten = (
                (buff._samplesWritten < leastSamplesWritten)
                ? buff._samplesWritten
                : leastSamplesWritten
            );
        leastSamplesProcessed = (
                (buff._samplesProcessed < leastSamplesProcessed)
                ? buff._samplesProcessed
                : leastSamplesProcessed
            );
        leastSamplesUnread = (
                (buff._samplesUnread < leastSamplesUnread)
                ? buff._samplesUnread
                : leastSamplesUnread
            );
    }
    this->_buffered = leastBuffered;
    this->_samplesWritten = leastSamplesWritten;
    this->_samplesUnwritten = this->_bufferLength - this->_samplesWritten;
    this->_samplesProcessed = leastSamplesProcessed;
    this->_samplesUnread = leastSamplesUnread;
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::_increment_counters()
{
    if (!this->_samplesUnwritten) Base<T, I>::rotate_write_buffer();
    if (!this->_samplesUnread) Base<T, I>::rotate_read_buffer();
    if (!Base<T, I>::unprocessed()) Base<T, I>::rotate_processing_buffer();
}

template <typename T, typename I>
void MultiRingBuffer<T, I>::update()
{
    _update_sample_counters();
    _increment_counters();
}

template <typename T, typename I>
inline int_fast32_t MultiRingBuffer<T, I>::buffered()
{
    update();
    return Base<T, I>::buffered();
}

template <typename T, typename I>
inline int_fast32_t MultiRingBuffer<T, I>::processed()
{
    update();
    return Base<T, I>::processed();
}

template <typename T, typename I>
std::vector<T> MultiRingBuffer<T, I>::read_interleaved()
{
    std::vector<T> output(this->_bufferLength);
    read_interleaved(&output);
    return output;
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_interleaved(std::vector<T>* data)
{
    int_fast8_t buffIndex(0);

    for (int_fast32_t i(0); i < this->_bufferLength; ++i)
    {
        #if _DEBUG
        this->buffers.at(buffIndex).read_samples(&(data->at(i)), 1);
        #else
        this->buffers[buffIndex].read_samples(&((*data)[i]), 1);
        #endif
        ++buffIndex %= this->_numBuffers;
    }
    update();
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_samples_interleaved(
        T* data,
        int_fast32_t length
    )
{
    #if _DEBUG
    if (length > (this->_bufferLength * this->_numBuffers))
    {
        throw std::out_of_range(
                "Length must be <= buffer length * num buffers"
            );
    }
    #endif

    int_fast8_t buffIndex(0);

    for (int_fast32_t i(0); i < length; ++i)
    {
        #if _DEBUG
        this->buffers.at(buffIndex).read_samples(data + i, 1);
        #else
        this->buffers[buffIndex].read_samples(data + i, 1);
        #endif
        ++buffIndex %= this->_numBuffers;
    }
    update();
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_bytes_interleaved(
        uint8_t* data,
        int_fast32_t numBytes
    )
{
    read_samples_interleaved(
            reinterpret_cast<T*>(data),
            numBytes / Base<T, I>::bytesPerSample
        );
}

template <typename T, typename I>
std::vector<T> MultiRingBuffer<T, I>::read_concatenated()
{
    std::vector<T> output(this->_bufferLength);
    read_concatenated(&output);
    return output;
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_concatenated(std::vector<T>* data)
{
    int_fast32_t
        sampleIndex(0),
        numSamples(this->_bufferLength / this->_numBuffers);

    for (int_fast8_t i(0); i < this->_numBuffers; ++i)
    {
        #if _DEBUG
        this->buffers.at(i).read_samples(
                &(data->at(sampleIndex)),
                numSamples
            );
        #else
        this->buffers[i].read_samples(
                &((*data)[sampleIndex]),
                numSamples
            );
        #endif
        sampleIndex += numSamples;
    }
    update();
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_samples_concatenated(
        T* data,
        int_fast32_t length
    )
{
    #if _DEBUG
    if (length > (this->_bufferLength * this->_numBuffers))
    {
        throw std::out_of_range(
                "Length must be <= buffer length * num buffers"
            );
    }
    #endif

    int_fast32_t
        sampleIndex(0),
        numSamples(length / this->_numBuffers);

    for (int_fast8_t i(0); i < this->_numBuffers; ++i)
    {
        #if _DEBUG
        this->buffers.at(i).read_samples(data + sampleIndex, numSamples);
        #else
        this->buffers[i].read_samples(data + sampleIndex, numSamples);
        #endif
        sampleIndex += numSamples;
    }
    update();
}

template <typename T, typename I>
inline void MultiRingBuffer<T, I>::read_bytes_concatenated(
        uint8_t* data,
        int_fast32_t numBytes
    )
{
    read_samples_concatenated(
            reinterpret_cast<T*>(data),
            numBytes / Base<T, I>::bytesPerSample
        );
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::write(T data, bool force)
{
    #if _DEBUG
    if (!size_is_set())
    {
        std::cerr << "Error: size not set!\n";
        throw SIZE_NOT_SET;
    }
    #endif

    for (RingBuffer<T, I>& buff: this->buffers)
    {
        #if _DEBUG
        if (buff.write(data, force) > 1)
        {
            throw std::out_of_range("Must not be > 1 sample");
        }
        #else
        buff.write(data, force);
        #endif
    }

    update();

    return 1;
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::write(
        std::vector<T> data,
        bool force
    )
{
    #if _DEBUG
    if (!size_is_set())
    {
        std::cerr << "Error: size not set!\n";
        throw SIZE_NOT_SET;
    }
    #endif

    int_fast32_t written(Base<T, I>::buffer_length()), common(written);
    for (RingBuffer<T, I>& buff: this->buffers)
    {
        written = buff.write(data, force);
        common = (written < common) ? written : common;
    }

    update();

    return common;
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::write_samples(
        T* data,
        int_fast32_t length,
        bool force
    )
{
    #if _DEBUG
    if (!size_is_set())
    {
        std::cerr << "Error: size not set!\n";
        throw SIZE_NOT_SET;
    }
    #endif

    int_fast32_t written(Base<T, I>::buffer_length()), common(written);

    for (RingBuffer<T, I>& buff: this->buffers)
    {
        written = buff.write_samples(data, length, force);
        common = (written < common) ? written : common;
    }

    update();

    return common;
}

template <typename T, typename I>
int_fast32_t MultiRingBuffer<T, I>::write_bytes(
        uint8_t* data,
        int_fast32_t numBytes,
        bool force
    )
{
    #if _DEBUG
    if (!size_is_set())
    {
        std::cerr << "Error: size not set!\n";
        throw SIZE_NOT_SET;
    }
    #endif

    int_fast32_t written(numBytes), common(written);

    for (RingBuffer<T, I>& buff: this->buffers)
    {
        written = buff.write_bytes(data, numBytes, force);
        common = (written < common) ? written : common;
    }

    update();

    return common;
}

template <typename T>
NonAtomicMultiRingBuffer<T>::NonAtomicMultiRingBuffer() :
MultiRingBuffer<T, int_fast8_t>()
{
}

template <typename T>
NonAtomicMultiRingBuffer<T>::NonAtomicMultiRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize,
        int_fast8_t numBuffers
    ) :
MultiRingBuffer<T, int_fast8_t>(bufferSize, ringSize, numBuffers)
{
}

template <typename T>
NonAtomicMultiRingBuffer<T>::NonAtomicMultiRingBuffer(
        const NonAtomicMultiRingBuffer& obj
    ) :
MultiRingBuffer<T, int_fast8_t>(obj)
{
}

template <typename T>
NonAtomicMultiRingBuffer<T>::~NonAtomicMultiRingBuffer()
{
}

template <typename T>
AtomicMultiRingBuffer<T>::AtomicMultiRingBuffer() :
MultiRingBuffer<T, std::atomic_int_fast8_t>()
{
}

template <typename T>
AtomicMultiRingBuffer<T>::AtomicMultiRingBuffer(
        int_fast32_t bufferSize,
        int_fast8_t ringSize,
        int_fast8_t numBuffers
    ) :
MultiRingBuffer<T, std::atomic_int_fast8_t>(
        bufferSize,
        ringSize,
        numBuffers
    )
{
}

template <typename T>
AtomicMultiRingBuffer<T>::AtomicMultiRingBuffer(
        const AtomicMultiRingBuffer& obj
    ) :
MultiRingBuffer<T, std::atomic_int_fast8_t>(obj)
{
}

template <typename T>
AtomicMultiRingBuffer<T>::~AtomicMultiRingBuffer()
{
}

/*                           Ring Buffer                            */

// template class Buffer::MultiRingBuffer<int8_t, int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint8_t, int_fast8_t>;
// template class Buffer::MultiRingBuffer<int16_t, int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint16_t, int_fast8_t>;
// template class Buffer::MultiRingBuffer<int32_t, int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint32_t, int_fast8_t>;

// template class Buffer::MultiRingBuffer<int, int_fast8_t>;

// template class Buffer::MultiRingBuffer<float, int_fast8_t>;
// template class Buffer::MultiRingBuffer<double, int_fast8_t>;

// /*                    Atomic Indices Ring Buffer                    */

// template class Buffer::MultiRingBuffer<int8_t, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint8_t, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<int16_t, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint16_t, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<int32_t, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<uint32_t, std::atomic_int_fast8_t>;

// template class Buffer::MultiRingBuffer<int, std::atomic_int_fast8_t>;

// template class Buffer::MultiRingBuffer<float, std::atomic_int_fast8_t>;
// template class Buffer::MultiRingBuffer<double, std::atomic_int_fast8_t>;

// /*                           Ring Buffer                            */

// template class Buffer::NonAtomicMultiRingBuffer<int8_t>;
// template class Buffer::NonAtomicMultiRingBuffer<uint8_t>;
// template class Buffer::NonAtomicMultiRingBuffer<int16_t>;
// template class Buffer::NonAtomicMultiRingBuffer<uint16_t>;
// template class Buffer::NonAtomicMultiRingBuffer<int32_t>;
// template class Buffer::NonAtomicMultiRingBuffer<uint32_t>;

// template class Buffer::NonAtomicMultiRingBuffer<int>;

// template class Buffer::NonAtomicMultiRingBuffer<float>;
// template class Buffer::NonAtomicMultiRingBuffer<double>;

/*                    Atomic Indices Ring Buffer                    */

// template class Buffer::AtomicMultiRingBuffer<int8_t>;
template class Buffer::AtomicMultiRingBuffer<uint8_t>;
template class Buffer::AtomicMultiRingBuffer<int16_t>;
// template class Buffer::AtomicMultiRingBuffer<uint16_t>;
template class Buffer::AtomicMultiRingBuffer<int32_t>;
// template class Buffer::AtomicMultiRingBuffer<uint32_t>;

template class Buffer::AtomicMultiRingBuffer<int>;

template class Buffer::AtomicMultiRingBuffer<float>;
template class Buffer::AtomicMultiRingBuffer<double>;
