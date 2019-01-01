#ifndef PIDMAP_H
#define PIDMAP_H

#include <fstream>
#include <map>
#include <vector>

#define PID_NOT_INITIALIZED  0xFFFF

class pidinfo {
public:
    unsigned int nb_packet;
    bool has_pcr;
    bool has_pts;
    bool has_dts;
    char pattern;
    float percent;
};

class pidmap
{
    // pid map
    std::map<unsigned int, pidinfo>::iterator m_pidMap_ii;
    std::map<unsigned int, pidinfo> m_pidMap;
    unsigned int m_prev_pid;

    // pid vec
    std::vector<unsigned int>::iterator m_pidVec_ii;
    std::vector<unsigned int> m_pidVec;
    unsigned char m_prev_pattern;

    std::ifstream m_fileIn;

public:
    pidmap(std::string& fileIn);
    ~pidmap();

    bool run(unsigned int NbPacket = (unsigned int)-1);

    void getPcrPid(std::vector<unsigned int>& pidVector);
    void getPtsPid(std::vector<unsigned int>& pidVector);
    void getDtsPid(std::vector<unsigned int>& pidVector);
    bool GetNextPidInfo(unsigned int& pid, pidinfo& pidInfo);
    bool GetNextPattern(unsigned char& pattern);
};

#endif // PIDMAP_H
