#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <iostream>
#include <fstream>
#include <map>

#define TIMESTAMP_NO_PID  0xFFFF

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

    // pcr next
    unsigned int m_pcr_prev_val;
    std::map<unsigned int, double>::iterator m_pcr_ii;

    // pts next
    unsigned int m_pts_prev_val;
    std::map<unsigned int, double>::iterator m_pts_ii;

    // dts next
    unsigned int m_dts_prev_val;
    std::map<unsigned int, double>::iterator m_dts_ii;

    // pcr pts dts delta next
    std::map<unsigned int, double> *m_delta_map;
    std::map<unsigned int, double>::iterator m_delta_ii;
    double m_delta_prev_val;

    // pcr jitter next
    std::map<unsigned int, double>::iterator m_jitter_ii;
    unsigned int m_jitter_prev_index;
    double m_jitter_prev_val;

    // timestamp diff
    std::map<unsigned int, double> *m_diff_map1, *m_diff_map2;
    std::map<unsigned int, double>::iterator m_diff_ii;
    unsigned int m_diff_prev_index;
    double m_diff_prev_value;


    double getMaxDeltaPcr();

public:

    timestamp(std::ifstream& fileIn, unsigned int pidpcr, unsigned int pidpts = TIMESTAMP_NO_PID, unsigned int piddts = TIMESTAMP_NO_PID);
    ~timestamp();

    double  getBitrate();
    double  getDuration();
    double  getPcr(unsigned int index);
    double  getPts(unsigned int index);
    double  getDts(unsigned int index);
    bool    getNextPcr(unsigned int& index, double& pcr);
    bool    getNextPts(unsigned int& index, double& pts);
    bool    getNextDts(unsigned int& index, double& dts);
    bool    getNextDelta(unsigned int& index, double& delta);
    bool    getNextJitterPcr(unsigned int& index, double& jitter);
    bool    getNextDiff(unsigned int& index, double& diff);
};


#endif // TIMESTAMP_H
