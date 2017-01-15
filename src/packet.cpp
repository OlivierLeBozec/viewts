#include "packet.h"

packet::packet(unsigned char* data):
    m_data(data),
    m_pcrprev(0)
{
    if (data[0] != 0x47) {
        std::cerr << "Sync byte mismatch" << std::endl;
        return;
    }
}

packet::AdaptationField_t packet::getAdaptationField(void)
{
    return (AdaptationField_t) ((m_data[3] & 0x30) >> 4);
}

bool packet::hasPes(void)
{
    // pusi
    if ((m_data[1] & 0x40) == 0x40) {
        int offset = getPesOffset();

        // pes start code 0x0000001
        if (m_data[offset] == 0x00 && m_data[offset+1] == 0x00 && m_data[offset+2] == 0x01) {
            return true;
        }
    }
    return false;
}

unsigned int packet::getPesOffset(void)
{
    // get adaptation field length
    int offset = 0;
    if (getAdaptationField() == FOLLOWED_BY_PAYLOAD) {
        offset = m_data[4]; // adaptation filed length
        ++offset;           // add adaptation field length byte
    }
    return offset+4;
}

unsigned int packet::getPid(void)
{
    return ((m_data[1] & 0x1f) << 8) | m_data[2];
}

bool packet::hasPcr(void)
{
    // check if adaptation field present
    AdaptationField_t adaptationField = getAdaptationField();
    if (adaptationField == NO_PAYLOAD || adaptationField == FOLLOWED_BY_PAYLOAD)
    {
        // adaptation field length
        if (m_data[4] == 0)
            return false;

        // check if pcr flag in adaptation field
        if ((m_data[5] & 0x10) == 0x10)
            return true;
        return false;
    }
    return false;
}

double packet::getPcr(void)
{
    if (!hasPcr()) return 0;

    // get pcr
    unsigned long long pcr = 0;

    /*printf("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
           m_data[0], m_data[1], m_data[2], m_data[3], m_data[4],
           m_data[5], m_data[6], m_data[7], m_data[8], m_data[9]);*/

    // unit of 90kHz
    pcr |= ((unsigned long long)m_data[6] << 25);
    pcr |= ((unsigned long long)m_data[7] << 17);
    pcr |= ((unsigned long long)m_data[8] << 9);
    pcr |= ((unsigned long long)m_data[9] << 1);
    pcr |= (m_data[10] & 0x80) >> 7;

    // unit of 27MHz
    pcr *= 300;
    pcr |= (m_data[10] & 0x01) << 8;
    pcr |= m_data[11];

    // pcr overlap - not handle
    /*if (m_pcrprev && ((pcr - m_pcrprev) > PCR_MAX))
        pcr += PCR_MAX;
    m_pcrprev = pcr;*/

    // unit of 27Mhz -> s
    return (double)pcr/27000000;
}
