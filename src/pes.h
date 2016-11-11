#ifndef PES_H
#define PES_H

#include <iostream>

class pes
{
    enum PTS_DTS_flags_t {
        NONE,
        RESERVED,
        HAS_PTS,
        HAS_PTS_DTS
    };
    PTS_DTS_flags_t getPtsDtsFlags();

    unsigned char*      m_data;
    unsigned int        m_pes_length;

public:
    double  getPts(void);
    double  getDts(void);
    bool    hasPts(void);
    bool    hasDts(void);

    pes(unsigned char* data);
};

#endif // PES_PACKET_H
