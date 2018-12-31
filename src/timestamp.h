#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <iostream>
#include <fstream>
#include <map>

#define TIMESTAMP_NO_PID  0xFFFF

class timestamp
{
    std::ifstream m_fileIn;

    // number packets before the first pcr
    unsigned int m_packetBeforeFirstPcr;

    // number of packets after the last pcr
    unsigned int m_packetAfterLastPcr;

    // min max pcr and and corresponding index
    double m_min_pcr;
    double m_max_pcr;
    double m_globalBitrate;
    unsigned int m_min_index;
    unsigned int m_max_index;
    unsigned int m_pidpcr;
    unsigned int m_pidpts;
    unsigned int m_piddts;
    unsigned int m_nbPacket;

    // pid pcr pts dts map
    std::map<unsigned int, double> m_pcrMap;
    std::map<unsigned int, double> m_ptsMap;
    std::map<unsigned int, double> m_dtsMap;
    std::map<unsigned int, unsigned int> m_pesLengthMap;
    std::map<double, int> m_levelMap;

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

    // local bitrate
    unsigned int m_bitrate_prev_index_val;
    double m_bitrate_prev_pcr_val;

    // buffer level
    int m_level;
    std::map<unsigned int, unsigned int>::iterator m_length_ii;

    double getMaxDeltaPcr();

public:

    timestamp(std::string fileNameIn, unsigned int pidpcr, unsigned int pidpts = TIMESTAMP_NO_PID, unsigned int piddts = TIMESTAMP_NO_PID);

    double  getGlobalBitrate();
    double  getDuration();
    bool    getTimeFromIndex(unsigned int index, double& time);
    bool    getNextPcr(unsigned int& index, double& pcr);
    bool    getNextPts(unsigned int& index, double& pts);
    bool    getNextDts(unsigned int& index, double& dts);
    bool    getNextDelta(unsigned int& index, double& delta);
    bool    getNextJitterPcr(unsigned int& index, double& jitter);
    bool    getNextDiff(unsigned int& index, double& diff);
    bool    getNextBitrate(unsigned int& index, double& bitrate);
    bool    getNextLevel(unsigned int& index, double& level);
    bool    run(unsigned int NbPacket = (unsigned int)-1);
};


#endif // TIMESTAMP_H
