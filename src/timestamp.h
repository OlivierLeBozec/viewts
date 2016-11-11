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
    unsigned int m_pidpcr;
    unsigned int m_pidpts;
    unsigned int m_piddts;

    // pcr pts dts map
    std::map<unsigned int, double> m_pcrMap;
    std::map<unsigned int, double> m_ptsMap;
    std::map<unsigned int, double> m_dtsMap;

    double getMaxDeltaPcr();
    double getBitrate();
    double getDuration();

public:
    timestamp(std::ifstream& fileIn, unsigned int pidpcr, unsigned int pidpts, unsigned int piddts);
    ~timestamp();

    double getPcr(int index);
    double getPts(int index);
    double getDts(int index);

/*  double  PopPcr();
    double  PopPts();
    double  PopDts();*/

    void    DumpBitrate();
    void    DumpDuration();
    void    DumpPcr();
    void    DumpPts();
    void    DumpDts();
    void    DumpDelta();
    void    DumpJitterPcr();
    void    DumpDiff();
};

#define TIMESTAMP_NO_PID  -1

#endif // TIMESTAMP_H
