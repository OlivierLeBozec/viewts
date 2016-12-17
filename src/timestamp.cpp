#include "timestamp.h"
#include "packet.h"
#include "pes.h"

timestamp::timestamp(std::ifstream& fileIn, unsigned int pidpcr, unsigned int pidpts, unsigned int piddts):
    m_fileIn(fileIn),
    m_packetBeforeFirstPcr(0),
    m_packetAfterLastPcr(0),
    m_min_pcr(0xFFFFFFFFFFFFFFFF),
    m_max_pcr(0),
    m_min_index(0xFFFFFFFF),
    m_max_index(0),
    m_pidpcr(pidpcr),
    m_pidpts(pidpts),
    m_piddts(piddts),
    m_nbPacket(0),
    m_pcr_prev_val(-1),
    m_pts_prev_val(-1),
    m_dts_prev_val(-1),
    m_delta_prev_val(-1),
    m_jitter_prev_index(-1),
    m_jitter_prev_val(-1),
    m_diff_prev_index(-1),
    m_diff_prev_value(-1)
{
    // TODO: find first start byte (repeated 0x47 every 188)

    // loop on packet
    m_fileIn.clear();
    m_fileIn.seekg(0); // TODO

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

// Cpu consuming function
bool timestamp::run(unsigned int nbPacketToRead)
{
    unsigned char data[188];

    while (m_fileIn.read((char*)data, 188) && nbPacketToRead)
    {
        // create packet from buffer
        packet packet(data);

        // update number of packet and store pcr
        if (packet.getPid() == m_pidpcr)
        {
            if (packet.hasPcr()) {
                m_pcrMap[m_nbPacket] = packet.getPcr();
                m_packetAfterLastPcr=0;

                // store min pcr and corresponding index
                if (m_pcrMap[m_nbPacket] < m_min_pcr) {
                    m_min_pcr = m_pcrMap[m_nbPacket];
                    m_min_index = m_nbPacket;
                }

                // store max pcr and corresponding index
                if (m_pcrMap[m_nbPacket] > m_max_pcr) {
                    m_max_pcr = m_pcrMap[m_nbPacket];
                    m_max_index = m_nbPacket;
                }
            }
            else { /* pid without pcr */
                if (m_pcrMap.empty()) m_packetBeforeFirstPcr++;
                m_packetAfterLastPcr++;
            }
        }

        if (packet.getPid() == m_pidpts && packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                if(pes.hasPts()) m_ptsMap[m_nbPacket] = pes.getPts();
        }

        if (packet.getPid() == m_piddts && packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                if(pes.hasDts()) m_dtsMap[m_nbPacket] = pes.getDts();
        }

        // next packet
        ++m_nbPacket;
        --nbPacketToRead;
    }

    // return true if all expected packets are read
    if (nbPacketToRead == 0)
        return true;
    return false;
}

double timestamp::getMaxDeltaPcr()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    return m_max_pcr-m_min_pcr;
}

double timestamp::getBitrate()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    double delta_pcr =  getMaxDeltaPcr();

    // bitrate in byte per second
    double bitrate = (double)((m_max_index - m_min_index)*188);
    bitrate /= delta_pcr;
    bitrate *=8;

    return bitrate;
}

double timestamp::getDuration()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    double delta_pcr = getMaxDeltaPcr();

    // bitrate in byte per second
    double bitrate = getBitrate();

    // duration in seconds
    double duration = delta_pcr;
    duration += (m_packetBeforeFirstPcr*188*8)/bitrate;   // before first pcr
    duration += (m_packetAfterLastPcr*188*8)/bitrate;     // after last pcr

    return duration;
}

double timestamp::getPcr(unsigned int index) {

    double pcr = 0;

    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    try {
        pcr = m_pcrMap[index];
    }
    catch(...) {
        pcr = 0;
    }

    return pcr;
}

double timestamp::getPts(unsigned int index) {

    double pts = 0;

    // test if pcr found
    if (m_ptsMap.empty()) return 0;

    try {
        pts = m_ptsMap[index];
    }
    catch(...) {
        pts = 0;
    }

    return pts;
}

double timestamp::getDts(unsigned int index) {

    double dts = 0;

    // test if pcr found
    if (m_dtsMap.empty()) return 0;

    try {
        dts = m_dtsMap[index];
    }
    catch(...) {
        dts = 0;
    }

    return dts;
}

bool timestamp::getNextPcr(unsigned int& index, double& pcr)
{
    if (m_pcr_prev_val == -1) {

        m_pcr_ii = m_pcrMap.begin();
        m_pcr_prev_val = (*m_pcr_ii).second;
    }

    if (m_pcr_ii != m_pcrMap.end()) {

        index = (*m_pcr_ii).first;
        pcr = (*m_pcr_ii).second;
        ++m_pcr_ii;

        return true;
    }

    return false;
}

bool timestamp::getNextPts(unsigned int& index, double& pts)
{
    if (m_pts_prev_val == -1) {

        m_pts_ii = m_ptsMap.begin();
        m_pts_prev_val = (*m_pts_ii).second;
    }

    if (m_pts_ii != m_ptsMap.end()) {

        index = (*m_pts_ii).first;
        pts = (*m_pts_ii).second;
        ++m_pts_ii;

        return true;
    }

    return false;
}

bool timestamp::getNextDts(unsigned int& index, double& dts)
{
    if (m_dts_prev_val == -1) {

        m_dts_ii = m_dtsMap.begin();
        m_dts_prev_val = (*m_dts_ii).second;
    }

    if (m_dts_ii != m_dtsMap.end()) {

        index = (*m_dts_ii).first;
        dts = (*m_dts_ii).second;
        ++m_dts_ii;

        return true;
    }

    return false;
}

bool timestamp::getNextDelta(unsigned int& index, double& delta)
{
    if (m_delta_prev_val == -1) {

        if (m_pidpcr != TIMESTAMP_NO_PID) {
            m_delta_map = &m_pcrMap;
        }
        else if(m_pidpts != TIMESTAMP_NO_PID) {
            m_delta_map = &m_ptsMap;
        }
        else if(m_piddts != TIMESTAMP_NO_PID) {
            m_delta_map = &m_dtsMap;
        }
        else
            return false;

        m_delta_ii = m_delta_map->begin();
        m_delta_prev_val = (*m_delta_ii).second;
    }

    if (!m_delta_map->empty() && m_delta_ii != --m_delta_map->end())
    {
        // next
        ++m_delta_ii;

        // delta in seconds
        delta = (*m_delta_ii).second - m_delta_prev_val;
        index = (*m_delta_ii).first;

        m_delta_prev_val = (*m_delta_ii).second;
        return true;
    }

    return false;
}

bool timestamp::getNextJitterPcr(unsigned int& index, double& jitter)
{
    if (m_jitter_prev_val == -1 && m_jitter_prev_index == -1) {

        m_jitter_ii = m_pcrMap.begin();
        m_jitter_prev_index = (*m_jitter_ii).first;
        m_jitter_prev_val = (*m_jitter_ii).second;
    }

    if (m_jitter_ii != --m_pcrMap.end()) {

        // next
        ++m_jitter_ii;

        // estimated pcr is compute using bitrate and number of bit
        double offset = ((*m_jitter_ii).first - m_jitter_prev_index) * 188 * 8;
        double estimate_pcr = m_jitter_prev_val;
        estimate_pcr += offset/getBitrate();

        // jitter in s
        jitter = (*m_jitter_ii).second - estimate_pcr;
        index = (*m_jitter_ii).first;

        m_jitter_prev_index = (*m_jitter_ii).first;
        m_jitter_prev_val = (*m_jitter_ii).second;

        return true;
    }

    return false;
}

bool timestamp::getNextDiff(unsigned int& index, double& diff)
{
    if (m_diff_prev_index == -1 && m_diff_prev_value == -1) {

        if (m_pidpcr != TIMESTAMP_NO_PID) {
            m_diff_map1 = &m_pcrMap;

            // dts - pcr
            if(m_piddts != TIMESTAMP_NO_PID) {
                m_diff_map2 = &m_dtsMap;
            }
            // pts - pcr
            else if(m_pidpts != TIMESTAMP_NO_PID) {
                m_diff_map2 = &m_ptsMap;
            }
            else return false;
        }
        else if(m_piddts != TIMESTAMP_NO_PID) {
            m_diff_map1 = &m_dtsMap;

            // pts - dts
            if(m_pidpts != TIMESTAMP_NO_PID) {
                m_diff_map2 = &m_ptsMap;
            }
        }
        else return false;

        // init value
        m_diff_ii = m_diff_map1->begin();
        m_diff_prev_index = (*m_diff_ii).first;
        m_diff_prev_value = (*m_diff_ii).second;
    }

    while (m_diff_ii != --m_diff_map1->end()) {

        ++m_diff_ii;
        for (unsigned int _index = m_diff_prev_index; _index < (*m_diff_ii).first; ++_index)
        {
            if (m_diff_map2->find(_index) != m_diff_map2->end())
            {
                // diff between different timestamp
                // should be on the same packet or higher
                diff = m_diff_map2->find(_index)->second - m_diff_prev_value;
                index = _index;

                m_diff_prev_index = (*m_diff_ii).first;
                m_diff_prev_value = (*m_diff_ii).second;
                return true;
            }
        }

        m_diff_prev_index = (*m_diff_ii).first;
        m_diff_prev_value = (*m_diff_ii).second;
    }

    return false;
}
