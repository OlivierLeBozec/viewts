#include "timestamp.h"
#include "packet.h"
#include "pes.h"

timestamp::timestamp(std::ifstream& fileIn, unsigned int pid):
    m_packetBeforeFirstPcr(0),
    m_packetAfterLastPcr(0),
    m_min_pcr(0xFFFFFFFFFFFFFFFF),
    m_max_pcr(0),
    m_min_index(0xFFFFFFFF),
    m_max_index(0)
{
    unsigned int nbPacket = 0;

    // fill the output file with PCR
    unsigned char data[188];

    // TODO: find first start byte (repeated 0x47 every 188)

    // loop on packet
    fileIn.seekg(0); // TODO
    while (fileIn.read((char*)data, 188))
    {
        // create packet from buffer
        packet packet(data);

        // update number of packet and store pcr
        if (packet.getPid() == pid)
        {
            if (packet.hasPcr()) {
                m_pcrMap[nbPacket] = packet.getPcr();
                m_packetAfterLastPcr=0;

                // store min pcr and corresponding index
                if (m_pcrMap[nbPacket] < m_min_pcr) {
                    m_min_pcr = m_pcrMap[nbPacket];
                    m_min_index = nbPacket;
                }

                // store max pcr and corresponding index
                if (m_pcrMap[nbPacket] > m_max_pcr) {
                    m_max_pcr = m_pcrMap[nbPacket];
                    m_max_index = nbPacket;
                }
            }
            else { /* pid without pcr */
                if (m_pcrMap.empty()) m_packetBeforeFirstPcr++;
                m_packetAfterLastPcr++;
            }

            if (packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                if(pes.hasPts()) m_ptsMap[nbPacket] = pes.getPts();
                if(pes.hasDts()) m_dtsMap[nbPacket] = pes.getDts();
            }
        }

        // next packet
        ++nbPacket;
    }
}

timestamp::~timestamp()
{
    if (!m_pcrMap.empty()) {
        m_pcrMap.clear();
    }
    if (!m_ptsMap.empty()) {
        m_ptsMap.clear();
    }
    if (!m_dtsMap.empty()) {
        m_dtsMap.clear();
    }
}

float timestamp::getMaxDeltaPcr()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    return m_max_pcr-m_min_pcr;
}

float timestamp::getBitrate()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    float delta_pcr =  getMaxDeltaPcr();

    // bitrate in byte per second
    float bitrate = (float)((m_max_index - m_min_index)*188);
    bitrate /= delta_pcr;
    bitrate *=8;

    return bitrate;
}

float timestamp::getDuration()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    float delta_pcr = getMaxDeltaPcr();

    // bitrate in byte per second
    float bitrate = getBitrate();

    // duration in seconds
    float duration = delta_pcr;
    duration += (m_packetBeforeFirstPcr*188*8)/bitrate;   // before first pcr
    duration += (m_packetAfterLastPcr*188*8)/bitrate;     // after last pcr

    return duration;
}

void timestamp::OutDuration()
{
    float duration = getDuration();

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(3);

    if (duration) {
        std::cout << "Duration(s)" << std::endl;
        std::cout << duration << std::endl;
    }
    else
        std::cerr << "Timestamp issue" << std::endl;

    std::cout << std::endl;
}

void timestamp::OutBitrate()
{
    float bitrate = getBitrate();

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(3);

    if (bitrate) {
        std::cout.width(20);
        std::cout << "Bitrate(b/s)";
        std::cout.width(20);
        std::cout << "(Kb/s)";
        std::cout.width(20);
        std::cout << "(Mb/s)" << std::endl;
        std::cout.width(20);
        std::cout << bitrate;
        std::cout.width(20);
        std::cout << bitrate/1024;
        std::cout.width(20);
        std::cout << bitrate/1024/1024 << std::endl;
    }
    else
        std::cerr << "Timestamp issue" << std::endl;

    std::cout << std::endl;
}

void timestamp::OutPcr()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(10);
    std::cout << "PCR(s)" << std::endl;

    std::map<unsigned int, float>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(10);
        std::cout << (*ii).second << std::endl;
    }

    std::cout << std::endl;
}

void timestamp::OutPts()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(10);
    std::cout << "PTS(s)" << std::endl;

    std::map<unsigned int, float>::iterator ii;
    for (ii=m_ptsMap.begin(); ii!=m_ptsMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(10);
        std::cout << (*ii).second << std::endl;
    }

    std::cout << std::endl;
}

void timestamp::OutDts()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(10);
    std::cout << "DTS(s)" << std::endl;

    std::map<unsigned int, float>::iterator ii;
    for (ii=m_dtsMap.begin(); ii!=m_dtsMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(10);
        std::cout << (*ii).second << std::endl;
    }
}

void timestamp::OutDeltaPcr()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(10);
    std::cout << "Delta PCR(s)" << std::endl;

    unsigned long long prev_pcr = 0;
    std::map<unsigned int, float>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        if (prev_pcr != 0) {
            std::cout.width(10);
            std::cout << (*ii).first;
            std::cout.width(10);
            std::cout << (*ii).second - prev_pcr << std::endl;
        }
        prev_pcr = (*ii).second;
    }
}

void timestamp::OutJitterPcr()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(10);
    std::cout << "Jitter PCR(ns)" << std::endl;

    float bitrate = getBitrate();
    unsigned long long prev_pcr = 0;
    unsigned int prev_index = 0;

    std::map<unsigned int, float>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        if (prev_pcr && prev_index)
        {
            // estimate pcr is compute using 27000000 clock
            unsigned long long estimate_pcr = prev_pcr;
            unsigned long long offset = ((*ii).first - prev_index) * 188 * 8;
            offset *= 27000000;
            offset /= (unsigned long long)bitrate;
            estimate_pcr += offset;

            // jitter in ns
            float jitter = (*ii).second*1000000 - estimate_pcr*1000000;

            std::cout.width(10);
            std::cout << (*ii).first;
            std::cout.width(10);
            std::cout << jitter << std::endl;
        }

        prev_index = (*ii).first;
        prev_pcr = (*ii).second;
    }
}

void timestamp::OutBufferLevel()
{
    float bitrate = getBitrate();

    std::cout.flags ( std::ios_base::fixed | std::ios::left);
    std::cout.precision(10);

    std::cout << std::endl;
    std::cout.width(20);
    std::cout << "Buffer Level(s)";
    std::cout.width(20);
    std::cout << "(Kb)" << std::endl;

    std::map<unsigned int, float>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        if (m_ptsMap[(*ii).first]) {
            float bufferLevel = m_ptsMap[(*ii).first] - (*ii).second;

            std::cout.width(20);
            std::cout << bufferLevel;
            std::cout.width(20);
            std::cout << bufferLevel*bitrate/1024;
            std::cout << std::endl;
        }
    }
}
