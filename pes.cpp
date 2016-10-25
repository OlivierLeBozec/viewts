#include "pes.h"

pes::pes(unsigned char *data) :
    m_data(data),
    m_pes_length(0)
{
    // pes start code 0x0000001
    if (data[0] != 0x00 || data[1] != 0x00 || data[2] != 0x01) {
        std::cerr << "Pes start code mismatch" << std::endl;
        return;
    }

    m_pes_length = data[4] << 8 | data[5];
}

pes::PTS_DTS_flags_t pes::getPtsDtsFlags()
{
    return (PTS_DTS_flags_t)(m_data[7] >> 6);
}

bool pes::hasPts(void)
{
   if (getPtsDtsFlags() == HAS_PTS || getPtsDtsFlags() == HAS_PTS_DTS)
       return true;
   return false;
}

float pes::getPts(void)
{
    unsigned long long pts = 0;

    if (hasPts()) {
        pts = (m_data[9] & 0x0E) >> 1;
        pts <<= 8;
        pts |= m_data[10];
        pts <<= 7;
        pts |= (m_data[11] & 0xFE) >> 1;
        pts <<= 8;
        pts |= m_data[12];
        pts <<= 7;
        pts |= (m_data[13] & 0xFE) >> 1;
    }

    // unit of 90Khz -> ms
    return (float)(pts/90000);
}

bool pes::hasDts(void)
{
   if (getPtsDtsFlags() == HAS_PTS_DTS)
       return true;
   return false;
}

float pes::getDts(void)
{
    unsigned long long dts = 0;

    if (hasDts()) {
        dts = (m_data[14] & 0x0E) >> 1;
        dts <<= 8;
        dts |= m_data[15];
        dts <<= 7;
        dts |= (m_data[16] & 0xFE) >> 1;
        dts <<= 8;
        dts |= m_data[17];
        dts <<= 7;
        dts |= (m_data[18] & 0xFE) >> 1;
    }

    // unit of 90Khz -> ms
    return (float)(dts/90000);
}
