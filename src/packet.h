#ifndef PACKET_H
#define PACKET_H

#include <iostream>

class packet
{
    static const unsigned long long PCR_MAX = /*2576980377811UL*/2^33*300;

    enum AdaptationField_t {
        RESERVED,
        NONE,
        NO_PAYLOAD,
        FOLLOWED_BY_PAYLOAD
    };
    AdaptationField_t   getAdaptationField(void);

    unsigned char*      m_data;
    unsigned long long  m_pcrprev;

public:
    unsigned int        getPid(void);
    double              getPcr(void);
    bool                hasPcr(void);
    bool                hasRap(void);
    bool                hasDiscontinuity(void);
    bool                hasPesHeader(void);
    unsigned int        getPesOffset(void);
    unsigned int        getCC(void);

    packet(unsigned char* data);
};


#endif // PACKET_H
