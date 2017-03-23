#include "timestamp.h"
#include "packet.h"
#include "pes.h"
#include "assert.h"

timestamp::timestamp(std::string* fileNameIn, unsigned int pidpcr, unsigned int pidpts, unsigned int piddts):
    m_packetBeforeFirstPcr(0),
    m_packetAfterLastPcr(0),
    m_min_pcr(0xFFFFFFFFFFFFFFFF),
    m_max_pcr(0),
    m_globalBitrate(0),
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
    m_diff_prev_value(-1),
    m_bitrate_prev_index_val(-1),
    m_bitrate_prev_pcr_val(-1),
    m_level(-1)
{
    m_fileIn =  new std::ifstream(fileNameIn->c_str(), std::ios::binary);

    // align file on first 0x47
    char start[512];
    int index = 0;

    m_fileIn->read((char*)start, 512);
    while (start[index] != 0x47 && start[index+188] != 0x47 && (index+188) < 512) index++;

    // loop on packet
    m_fileIn->clear();
    m_fileIn->seekg(index);
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
    if (!m_pesLengthMap.empty()) {
        m_pesLengthMap.clear();
    }
    m_fileIn->close();
    delete m_fileIn;
}

// Cpu consuming function
bool timestamp::run(unsigned int nbPacketToRead)
{
    unsigned char data[188];
    bool isDatatoRead = false;

    while (nbPacketToRead)
    {
        // leave if no more data
        if (!m_fileIn->read((char*)data, 188)) break;
        isDatatoRead = true;

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
                m_pesLengthMap[m_nbPacket] = pes.getPacketLength();
                if(pes.hasPts()) m_ptsMap[m_nbPacket] = pes.getPts();
        }

        if (packet.getPid() == m_piddts && packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                m_pesLengthMap[m_nbPacket] = pes.getPacketLength();
                if(pes.hasDts()) m_dtsMap[m_nbPacket] = pes.getDts();
        }

        // next packet
        ++m_nbPacket;
        --nbPacketToRead;
    }

    // update global bitrate
    getGlobalBitrate();

    // return true if some data are uptated
    return isDatatoRead;
}

double timestamp::getMaxDeltaPcr()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    return m_max_pcr-m_min_pcr;
}

// in byte per seconds
double timestamp::getGlobalBitrate()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    double delta_pcr =  getMaxDeltaPcr();

    // bitrate in byte per second
    m_globalBitrate = (double)((m_max_index - m_min_index)*188);
    m_globalBitrate /= delta_pcr;

    return m_globalBitrate;
}

double timestamp::getDuration()
{
    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    // max pcr diff in seconds
    double delta_pcr = getMaxDeltaPcr();

    // duration in seconds
    double duration = delta_pcr;
    duration += (m_packetBeforeFirstPcr*188)/m_globalBitrate;   // before first pcr
    duration += (m_packetAfterLastPcr*188)/m_globalBitrate;     // after last pcr

    return duration;
}

double timestamp::getTimeFromIndex(unsigned int index)
{
    double time = 0;

#if 1
    if (m_pcrMap.find(index) != m_pcrMap.end()) {
        // use current pcr value
        time = m_pcrMap[index] - (*m_pcrMap.begin()).second;
    }
    else {
        // lower pcr
        std::map<unsigned int, double>::iterator itlow;
        unsigned int index_low = index;
        do {
            itlow = m_pcrMap.find(index_low--);
        } while (itlow == m_pcrMap.end() && index_low > 0);

        // upper pcr
        std::map<unsigned int, double>::iterator ithigh;
        ithigh = m_pcrMap.lower_bound(index);

        if (ithigh != m_pcrMap.end() && itlow != m_pcrMap.end())
        {
            // calculate mean bitrate
            double localBitrate = ithigh->first - itlow->first;
            localBitrate *= 188;
            localBitrate /= ithigh->second - itlow->second;
            //printf("localbitrate %f\n", localBitrate);

            // estimate time
            time = ((index - itlow->first)*188)/localBitrate;
            time += itlow->second;
            time -= (*m_pcrMap.begin()).second;
        }
    }

    return time;

#else
    return (index*188)/m_globalBitrate + (*m_pcrMap.begin()).second;
#endif
}

bool timestamp::getNextPcr(unsigned int& index, double& pcr)
{
    // protection
    if (m_pcrMap.empty())
        return false;

    // init iterator
    if (m_pcr_prev_val == -1) {

        m_pcr_ii = m_pcrMap.begin();
        index = m_pcr_ii->first;
        m_pcr_prev_val = pcr = m_pcr_ii->second;
        return true;
    }

    if (m_pcr_ii != --m_pcrMap.end()) {

        ++m_pcr_ii;
        index = m_pcr_ii->first;
        m_pcr_prev_val = pcr = m_pcr_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextPts(unsigned int& index, double& pts)
{
    // protection
    if (m_ptsMap.empty())
        return false;

    // init iterator
    if (m_pts_prev_val == -1) {

        m_pts_ii = m_ptsMap.begin();
        index = m_pts_ii->first;
        m_pts_prev_val = pts = m_pts_ii->second;
        return true;
    }

    if (m_pts_ii != --m_ptsMap.end()) {

        ++m_pts_ii;
        index = m_pts_ii->first;
        m_pts_prev_val = pts = m_pts_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextDts(unsigned int& index, double& dts)
{
    // protection
    if (m_dtsMap.empty())
        return false;

    // init iterator
    if (m_dts_prev_val == -1) {

        m_dts_ii = m_dtsMap.begin();
        index = m_dts_ii->first;
        m_dts_prev_val = dts = m_dts_ii->second;
        return true;
    }

    if (m_dts_ii != --m_dtsMap.end()) {

        ++m_dts_ii;
        index = m_dts_ii->first;
        m_dts_prev_val = dts = m_dts_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextDelta(unsigned int& index, double& delta)
{
    if (m_delta_prev_val == -1) {

        if(m_piddts != TIMESTAMP_NO_PID) {
            m_delta_map = &m_dtsMap;
        }
        else if(m_pidpts != TIMESTAMP_NO_PID) {
            m_delta_map = &m_ptsMap;
        }
        else if (m_pidpcr != TIMESTAMP_NO_PID) {
            m_delta_map = &m_pcrMap;
        }
        else
            return false;

        m_delta_ii = m_delta_map->begin();
        m_delta_prev_val = m_delta_ii->second;
    }

    if (!m_delta_map->empty() && m_delta_ii != --m_delta_map->end())
    {
        // next
        ++m_delta_ii;

        // delta in seconds
        delta = m_delta_ii->second - m_delta_prev_val;
        index = m_delta_ii->first;

        m_delta_prev_val = m_delta_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextJitterPcr(unsigned int& index, double& jitter)
{
    if (m_jitter_prev_val == -1 && m_jitter_prev_index == -1) {

        m_jitter_ii = m_pcrMap.begin();
        m_jitter_prev_index = m_jitter_ii->first;
        m_jitter_prev_val = m_jitter_ii->second;
    }

    if (m_jitter_ii != --m_pcrMap.end()) {

        // next
        ++m_jitter_ii;

        // estimated pcr is compute using bitrate and number of bit
        double offset = (m_jitter_ii->first - m_jitter_prev_index) * 188;
        double estimate_pcr = m_jitter_prev_val;
        estimate_pcr += offset/m_globalBitrate;

        // jitter in s
        jitter = m_jitter_ii->second - estimate_pcr;
        index = m_jitter_ii->first;

        m_jitter_prev_index = m_jitter_ii->first;
        m_jitter_prev_val = m_jitter_ii->second;

        return true;
    }

    return false;
}

bool timestamp::getNextDiff(unsigned int& index, double& diff)
{
    if (m_diff_prev_index == -1 && m_diff_prev_value == -1) {

        if(m_piddts != TIMESTAMP_NO_PID && m_pidpts != TIMESTAMP_NO_PID) {

            // pts - dts
            m_diff_map1 = &m_dtsMap;
            m_diff_map2 = &m_ptsMap;
        }
        else if (m_pidpcr != TIMESTAMP_NO_PID && m_piddts != TIMESTAMP_NO_PID) {

            // dts - pcr
            m_diff_map1 = &m_pcrMap;
            m_diff_map2 = &m_dtsMap;
        }
        else if (m_pidpcr != TIMESTAMP_NO_PID && m_pidpts != TIMESTAMP_NO_PID) {

            // pts - pcr
            m_diff_map1 = &m_pcrMap;
            m_diff_map2 = &m_ptsMap;
        }
        else return false;

        // init value
        m_diff_ii = m_diff_map1->begin();
        m_diff_prev_index = m_diff_ii->first;
        m_diff_prev_value = m_diff_ii->second;
    }

    while (m_diff_ii != --m_diff_map1->end()) {

        ++m_diff_ii;
        for (unsigned int _index = m_diff_prev_index; _index < m_diff_ii->first; ++_index)
        {
            if (m_diff_map2->find(_index) != m_diff_map2->end())
            {
                // diff between different timestamp
                // should be on the same packet or higher
                diff = m_diff_map2->find(_index)->second - m_diff_prev_value;
                index = _index;

                m_diff_prev_index = m_diff_ii->first;
                m_diff_prev_value = m_diff_ii->second;
                return true;
            }
        }

        m_diff_prev_index = m_diff_ii->first;
        m_diff_prev_value = m_diff_ii->second;
    }

    return false;
}

bool timestamp::getNextBitrate(unsigned int& index, double& bitrate)
{
    // protection
    if (m_pcrMap.empty())
        return false;

    // init iterator
    if (m_bitrate_prev_pcr_val == -1) {

        m_pcr_ii = m_pcrMap.begin();
        m_bitrate_prev_index_val = m_pcr_ii->first;
        m_bitrate_prev_pcr_val = m_pcr_ii->second;
    }

    if (m_pcr_ii != --m_pcrMap.end()) {

        ++m_pcr_ii;
        bitrate = m_pcr_ii->first - m_bitrate_prev_index_val;
        bitrate *= 188;
        bitrate /= m_pcr_ii->second - m_bitrate_prev_pcr_val;
        index = m_pcr_ii->first;
        m_bitrate_prev_index_val = m_pcr_ii->first;
        m_bitrate_prev_pcr_val = m_pcr_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextLevel(unsigned int& index, double& level)
{
    int _level = 0;

    // protection
    if (m_pesLengthMap.empty())
        return false;

    // init index and level
    if (m_level == -1) {

        m_length_ii = m_pesLengthMap.begin();
        index = m_length_ii->first;
        _level = m_level = m_length_ii->second;
    }
    else if (m_length_ii != --m_pesLengthMap.end()) {

        ++m_length_ii;
        index = m_length_ii->first;
        _level = m_length_ii->second;
    }
    else return false;

    // time when buffer is release
    double release_time = 0;
    std::map<unsigned int, double>::const_iterator dts_ii = m_dtsMap.find(index);
    std::map<unsigned int, double>::const_iterator pts_ii = m_ptsMap.find(index);
    if (dts_ii != m_dtsMap.end())
        release_time = m_dtsMap[index];
    else if (pts_ii != m_ptsMap.end())
        release_time = m_ptsMap[index];

    // should never occur -> program stop TBC
    assert(release_time != 0);

    // increase buffer and set release time
    m_level += _level;
    m_levelMap[release_time] = _level;

    // decrease buffer level - remove old buffer
    std::map<double, int>::iterator level_ii = m_levelMap.begin();
    while (level_ii != m_levelMap.end())
    {
        if (level_ii->first < getTimeFromIndex(index) && level_ii->second) {
            //printf ("Del %f %u\n", level_ii->first, level_ii->second);
            m_level -= level_ii->second;
            level_ii->second = 0;
        }

        if (level_ii != m_levelMap.end()) level_ii++;
    }

    //printf ("Nb item %u - %f\n", m_levelMap.size(), getTimeFromIndex(index));
    level = (double)m_level;
    return true;
}
