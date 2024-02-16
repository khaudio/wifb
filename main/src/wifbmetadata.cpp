#include "wifbmetadata.h"

WIFBMetadata::WIFBMetadata()
{
}

WIFBMetadata::WIFBMetadata(const WIFBMetadata& obj)
{
    #if _DEBUG
    if (METADATA_SIZE < ((sizeof(int)) * 4))
    {
        throw METADATA_SIZE_TOO_SMALL;
    }
    #endif

    std::memcpy(this->data, obj.data, (METADATA_SIZE));
}

WIFBMetadata::~WIFBMetadata()
{
}

void WIFBMetadata::_set_data_from_timecode(void)
{
    for (int i(0); i < 4; ++i)
    {
        this->data[(i * sizeof(int))] = this->timecode[i];
    }
}

void WIFBMetadata::_set_timecode_from_data(void)
{
    for (int i(0); i < 4; ++i)
    {
        this->timecode[i] = (
                *reinterpret_cast<int*>(&(this->data[(i * sizeof(int))]))
            );
    }
}

void WIFBMetadata::set_timecode(std::array<int, 4> tc)
{
    std::copy(tc.begin(), tc.end(), this->timecode.begin());
    _set_data_from_timecode();
}

void WIFBMetadata::set_timecode(int hr, int min, int sec, int frm)
{
    this->timecode[0] = hr;
    this->timecode[1] = min;
    this->timecode[2] = sec;
    this->timecode[3] = frm;
    _set_data_from_timecode();
}

void WIFBMetadata::set_data(const uint8_t* incoming)
{
    std::memcpy(this->data, incoming, (METADATA_SIZE));
    _set_timecode_from_data();
}

void WIFBMetadata::get_data(uint8_t* outgoing) const
{
    std::memcpy(outgoing, this->data, (METADATA_SIZE));
}

std::array<int, 4> WIFBMetadata::get_timecode(void) const
{
    return this->timecode;
}
