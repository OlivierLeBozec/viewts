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
    double m_min_pcr;
    double m_max_pcr;
    unsigned int m_min_index;
    unsigned int m_max_index;

    // pcr pts dts map
    std::map<unsigned int, double> m_pcrMap;
    std::map<unsigned int, double> m_ptsMap;
    std::map<unsigned int, double> m_dtsMap;

    double getMaxDeltaPcr();
    double getBitrate();
    double getDuration();

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
