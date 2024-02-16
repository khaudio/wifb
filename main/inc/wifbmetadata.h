#ifndef WIFB_METADATA_H
#define WIFB_METADATA_H

#include "debugmacros.h"

#include <array>
#include <cstdint>
#include <cstring>

enum wifb_meta_err
{
    METADATA_SIZE_TOO_SMALL = -601,
};

/* Size in bytes of metadata for each transmission */
#ifndef METADATA_SIZE
#define METADATA_SIZE                       (128)
#endif

class WIFBMetadata
{

public:

    uint8_t data[METADATA_SIZE];
    std::array<int, 4> timecode = {0, 0, 0, 0};

public:

    WIFBMetadata();
    WIFBMetadata(const WIFBMetadata& obj);

    ~WIFBMetadata();

private:

    /* Set data chunk from timecode values */
    void _set_data_from_timecode(void);

    /* Extrapolate and set timecode
    from transmission metadata*/
    void _set_timecode_from_data(void);

public:

    /* Set timecode data chunk */
    void set_timecode(std::array<int, 4> tc);
    void set_timecode(int hr, int min, int sec, int frm);

    /* Copy data from external address */
    void set_data(const uint8_t* incoming);

    /* Copy data to external address */
    void get_data(uint8_t* outgoing) const;

    /* Return stored timecode value */
    std::array<int, 4> get_timecode(void) const;

};

#endif
