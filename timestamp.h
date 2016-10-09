#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <iostream>
#include <fstream>
#include <map>

class timestamp
{
    // number packets before the first pcr
    unsigned int m_packetBeforeFirstPcr;

    // number of packets after the last pcr
    unsigned int m_packetAfterLastPcr;

    // min max pcr and and corresponding index
    unsigned long long m_min_pcr;
    unsigned long long m_max_pcr;
    unsigned int m_min_index;
    unsigned int m_max_index;

    // pcr pts dts map
    std::map<unsigned int, unsigned long long> m_pcrMap;
    std::map<unsigned int, unsigned long long> m_ptsMap;
    std::map<unsigned int, unsigned long long> m_dtsMap;

    float getMaxDeltaPcr();
    float getBitrate();
    float getDuration();

public:
    timestamp(std::ifstream& fileIn, unsigned int pid);
    ~timestamp();

    void    OutBitrate();
    void    OutDuration();
    void    OutPcr();
    void    OutPts();
    void    OutDts();
    void    OutDeltaPcr();
    void    OutJitterPcr();
    void    OutBufferLevel();
};

#endif // TIMESTAMP_H
