#include "timestamp.h"
#include "packet.h"
#include "pes.h"
#include "assert.h"
#include <stdexcept>

timestamp::timestamp(std::string &fileNameIn, unsigned int pidpcr, unsigned int pidpts, unsigned int piddts):
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
    m_isCCerrorInit(false),
    m_isRAPinit(false),
    m_isPCRinit(false),
    m_isPTSinit(false),
    m_isDTSinit(false),
    m_delta_prev_val(TIMESTAMP_NOT_INITIALIZED),
    m_jitter_prev_index(TIMESTAMP_NOT_INITIALIZED),
    m_jitter_prev_val(TIMESTAMP_NOT_INITIALIZED),
    m_diff_map1(nullptr),
    m_diff_map2(nullptr),
    m_diff_prev_index(TIMESTAMP_NOT_INITIALIZED),
    m_diff_prev_value(TIMESTAMP_NOT_INITIALIZED),
    m_bitrate_prev_index_val(TIMESTAMP_NOT_INITIALIZED),
    m_bitrate_prev_pcr_val(TIMESTAMP_NOT_INITIALIZED),
    m_level(TIMESTAMP_NOT_INITIALIZED)
{
    m_fileIn.open(fileNameIn.c_str(), std::ios::binary);
    if(!m_fileIn.is_open())
      throw std::runtime_error("Can't open '" + fileNameIn + "' for reading");

    // align file on first 0x47
    char start[512];
    unsigned int index = 0;

    m_fileIn.read(start, sizeof start);
    while (start[index] != 0x47 && start[index+188] != 0x47 && (index+188) < (sizeof start)) index++;

    // loop on packet
    m_fileIn.clear();
    m_fileIn.seekg(index);
}

// Cpu consuming function
bool timestamp::run(unsigned int nbPacketToRead)
{
    unsigned char data[188];
    bool isDatatoRead = false;
    unsigned int startPacketIndex = 0;

    while (nbPacketToRead)
    {
        bool updatePesLength = false;

        // leave if no more data
        if (!m_fileIn.read((char*)data, 188)) break;
        isDatatoRead = true;

        // create packet from buffer
        packet packet(data);
        unsigned int pid = packet.getPid();

        // check flags
        if ( pid == m_pidpts || pid == m_piddts || pid == m_pidpcr )
        {
            // compare expected continuity counter to current - must be identical or +1
            if (((m_ccMap[pid] + 1) % 16 != packet.getCC()) &&
                (m_ccMap[pid] % 16 != packet.getCC()))
            {
                m_ccError[m_nbPacket] = pid;
            }
            m_ccMap[pid] = packet.getCC();

            // check RAP
            if (packet.hasRap())
            {
                m_rapMap[m_nbPacket] = pid;
            }
        }

        // update number of packet and store PCR
        if (pid == m_pidpcr)
        {
            if (packet.hasPcr()) {
                m_pcrMap[m_nbPacket] = packet.getPcr();
                m_packetAfterLastPcr=0;

                // store min PCR and corresponding index
                if (m_pcrMap[m_nbPacket] < m_min_pcr) {
                    m_min_pcr = m_pcrMap[m_nbPacket];
                    m_min_index = m_nbPacket;
                }

                // store max PCR and corresponding index
                if (m_pcrMap[m_nbPacket] > m_max_pcr) {
                    m_max_pcr = m_pcrMap[m_nbPacket];
                    m_max_index = m_nbPacket;
                }
            }
            else { // pid without PCR
                if (m_pcrMap.empty()) m_packetBeforeFirstPcr++;
                m_packetAfterLastPcr++;
            }
        }

        // get PTS and DTS
        if ((pid == m_pidpts || pid == m_piddts) && packet.hasPesHeader())
        {
            // create pes from buffer
            pes pes(data + packet.getPesOffset());

            if(pes.hasPts() && pid == m_pidpts) {
                m_ptsMap[m_nbPacket] = pes.getPts();
                updatePesLength = true;
            }

            if(pes.hasDts() && pid == m_piddts) {
                m_dtsMap[m_nbPacket] = pes.getDts();
                updatePesLength = true;
            }
        }

        // update PES length using timestamp as trigger - cause length is PES header can be null
        if ((pid == m_pidpts || pid == m_piddts) && updatePesLength == true)
        {
            startPacketIndex = m_nbPacket;
            m_pesLengthMap[startPacketIndex] = 184;
        }
        else if ((pid == m_pidpts || pid == m_piddts) && startPacketIndex != 0)
        {
            // add remaining PES size
            m_pesLengthMap[startPacketIndex] += 184;
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
    double delta_pcr = getMaxDeltaPcr();

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

bool timestamp::getTimeFromIndex(unsigned int index, double &time)
{
    if (m_pcrMap.find(index) != m_pcrMap.end()) {
        // use current pcr value
        time = m_pcrMap[index] - (*m_pcrMap.begin()).second;
        return true;
    }
    else // estimate time
    {
        // upper and lower pcr
        std::map<unsigned int, double>::iterator ithigh, itlow;
        itlow = ithigh = m_pcrMap.upper_bound(index);
        if (ithigh == m_pcrMap.begin() || ithigh == m_pcrMap.end())
            return false;
        --itlow;

        //printf("upper %u [%u] %llf\n", index, ithigh->first, ithigh->second);
        //printf("lower %u [%u] %llf\n", index, itlow->first, itlow->second);

        if (ithigh != m_pcrMap.end() && itlow != m_pcrMap.end())
        {
            // calculate mean bitrate
            double localBitrate = ithigh->first - itlow->first;
            localBitrate *= 188;
            localBitrate /= ithigh->second - itlow->second;
            //printf("localbitrate %llf\n", localBitrate);

            // estimate time
            time = ((index - itlow->first)*188)/localBitrate;
            time += itlow->second;
            time -= (*m_pcrMap.begin()).second;
            return true;
        }

        printf("error upper %u [%u] %f\n", index, ithigh->first, ithigh->second);
        printf("error lower %u [%u] %f\n", index, itlow->first, itlow->second);
    }

    return false;
}

bool timestamp::getNextPcr(unsigned int& index, double& pcr)
{
    // protection
    if (m_pcrMap.empty())
        return false;

    // init iterator
    if (m_isPCRinit == false)
    {
        m_pcr_ii = m_pcrMap.begin();
        index = m_pcr_ii->first;
        pcr = m_pcr_ii->second;
        m_isPCRinit = true;
        return true;
    }

    if (m_pcr_ii != --m_pcrMap.end())
    {
        ++m_pcr_ii;
        index = m_pcr_ii->first;
        pcr = m_pcr_ii->second;
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
    if (m_isPTSinit == false)
    {
        m_pts_ii = m_ptsMap.begin();
        index = m_pts_ii->first;
        pts = m_pts_ii->second;
        m_isPTSinit = true;
        return true;
    }

    if (m_pts_ii != --m_ptsMap.end())
    {
        ++m_pts_ii;
        index = m_pts_ii->first;
        pts = m_pts_ii->second;
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
    if (m_isDTSinit == false)
    {
        m_dts_ii = m_dtsMap.begin();
        index = m_dts_ii->first;
        dts = m_dts_ii->second;
        m_isDTSinit = true;
        return true;
    }

    if (m_dts_ii != --m_dtsMap.end())
    {
        ++m_dts_ii;
        index = m_dts_ii->first;
        dts = m_dts_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextDelta(unsigned int& index, double& delta)
{
    if (m_delta_prev_val == TIMESTAMP_NOT_INITIALIZED) {

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
    if (m_jitter_prev_val == TIMESTAMP_NOT_INITIALIZED && m_jitter_prev_index == TIMESTAMP_NOT_INITIALIZED) {

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
    if (m_diff_prev_index == TIMESTAMP_NOT_INITIALIZED && m_diff_prev_value == TIMESTAMP_NOT_INITIALIZED) {

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
    if (m_bitrate_prev_pcr_val == TIMESTAMP_NOT_INITIALIZED)
    {
        m_pcr_ii = m_pcrMap.begin();
        m_bitrate_prev_index_val = m_pcr_ii->first;
        m_bitrate_prev_pcr_val = m_pcr_ii->second;
    }

    if (m_pcr_ii != --m_pcrMap.end())
    {
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
    unsigned int levelTmp = 0;

    // protection
    if (m_pesLengthMap.empty())
        return false;

    // init index and level
    if (m_level == TIMESTAMP_NOT_INITIALIZED)
    {
        m_length_ii = m_pesLengthMap.begin();
        index = m_length_ii->first;
        levelTmp = m_level = m_length_ii->second;
    }
    else if (m_length_ii != --m_pesLengthMap.end())
    {
        ++m_length_ii;
        index = m_length_ii->first;
        levelTmp = m_length_ii->second;
    }
    else return false;

    // time when buffer is release
    double release_time = 0;
    std::map<unsigned int, double>::const_iterator dts_ii = m_dtsMap.find(index);
    std::map<unsigned int, double>::const_iterator pts_ii = m_ptsMap.find(index);
    if (dts_ii != m_dtsMap.end())
        release_time = m_dtsMap[index] - (*m_pcrMap.begin()).second;
    else if (pts_ii != m_ptsMap.end())
        release_time = m_ptsMap[index] - (*m_pcrMap.begin()).second;

    // should never occur -> program stop TBC
    assert(release_time != 0);

    double current_time;
    if (getTimeFromIndex(index, current_time) == true)
    {
        // increase buffer and set release time
        m_level += levelTmp;
        m_levelMap[release_time] = levelTmp;

        // decrease buffer level - remove old buffer
        std::map<double, unsigned int>::iterator level_ii = m_levelMap.begin();
        for (;level_ii != m_levelMap.end(); ++level_ii)
        {
            // compare release time with current time
            if (level_ii->first < current_time && level_ii->second) {
                //printf ("Del %f %u\n", level_ii->first, level_ii->second);
                m_level -= level_ii->second;
                level_ii->second = 0;
            }
        }
    }

    level = static_cast<double>(m_level);
    //printf ("Nb item %u - %u\n", m_levelMap.size(), m_level);
    return true;
}

bool timestamp::getNextCC(unsigned int& index, unsigned int& pid)
{
    // protection
    if (m_ccError.empty())
        return false;

    // init iterator
    if (m_isCCerrorInit == false)
    {
        m_cc_ii = m_ccError.begin();
        index = m_cc_ii->first;
        pid = m_cc_ii->second;
        m_isCCerrorInit = true;
        return true;
    }

    if (m_cc_ii != --m_ccError.end())
    {
        ++m_cc_ii;
        index = m_cc_ii->first;
        pid = m_cc_ii->second;
        return true;
    }

    return false;
}

bool timestamp::getNextRap(unsigned int& index, unsigned int& pid)
{
    // protection
    if (m_rapMap.empty())
        return false;

    // init iterator
    if (m_isRAPinit == false)
    {
        m_rap_ii = m_rapMap.begin();
        index = m_rap_ii->first;
        pid = m_rap_ii->second;
        m_isRAPinit = true;
        return true;
    }

    if (m_rap_ii != --m_rapMap.end())
    {
        ++m_rap_ii;
        index = m_rap_ii->first;
        pid = m_rap_ii->second;
        return true;
    }

    return false;
}
