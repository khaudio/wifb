#include "wifbnetwork.h"

std::string mac_addr_string(uint8_t addr[6])
{
    std::ostringstream concatenated;
    concatenated << std::setfill('0');
    for (int i(0); i < 6; ++i)
    {
        concatenated << std::setw(2) << std::hex << +addr[i];
        concatenated << (i < 5 ? ":" : "");
    }
    return concatenated.str();
}

std::string ip_addr_string(uint8_t addr[4])
{
    std::ostringstream stream;
    for (int i(0); i < 4; ++i)
    {
        stream << +addr[i] << (i < 3 ? "." : "");
    }
    return stream.str();
}

std::string ip_addr_string(int addr)
{
    return ip_addr_string(reinterpret_cast<uint8_t*>(&addr));
}

std::string ip_addr_string(esp_ip4_addr_t addr)
{
    return ip_addr_string(reinterpret_cast<uint8_t*>(&addr));
}

bool match_mac_addr(const uint8_t addr1[6], const uint8_t addr2[6])
{
    for (int i(0); i < 6; ++i)
    {
        if (addr1[i] != addr2[i]) return false;
    }
    return true;
}
