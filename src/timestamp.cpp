#include "timestamp.h"
#include "packet.h"
#include "pes.h"

#define DUMP_PRECISION 12

timestamp::timestamp(std::ifstream& fileIn, unsigned int pidpcr, unsigned int pidpts, unsigned int piddts):
    m_packetBeforeFirstPcr(0),
    m_packetAfterLastPcr(0),
    m_min_pcr(0xFFFFFFFFFFFFFFFF),
    m_max_pcr(0),
    m_min_index(0xFFFFFFFF),
    m_max_index(0),
    m_pidpcr(pidpcr),
    m_pidpts(pidpts),
    m_piddts(piddts)
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
        if (packet.getPid() == pidpcr)
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
        }

        if (packet.getPid() == pidpts && packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                if(pes.hasPts()) m_ptsMap[nbPacket] = pes.getPts();
        }

        if (packet.getPid() == piddts && packet.hasPes()) {
                // create pes from buffer
                pes pes(&data[packet.getPesOffset()]);
                if(pes.hasDts()) m_dtsMap[nbPacket] = pes.getDts();
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

/*double timestamp::PopPcr() {

    // test if pcr found
    if (m_pcrMap.empty()) return 0;

    std::map<unsigned int, double>::iterator ii = m_pcrMap.begin();
    m_pcrMap.erase(ii);

    return (*ii).second;
}
*/

double timestamp::getPcr(int index) {

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

double timestamp::getPts(int index) {

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

double timestamp::getDts(int index) {

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

void timestamp::DumpDuration()
{
    double duration = getDuration();

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

void timestamp::DumpBitrate()
{
    double bitrate = getBitrate();

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

void timestamp::DumpPcr()
{
    // test if pcr found
    if (m_pcrMap.empty()) return;

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(DUMP_PRECISION);
    std::cout << "PCR(s)" << std::endl;

    std::map<unsigned int, double>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(DUMP_PRECISION);
        std::cout << (*ii).second << std::endl;
    }

    std::cout << std::endl;
}

void timestamp::DumpPts()
{
    // test if pcr found
    if (m_ptsMap.empty()) return;

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(DUMP_PRECISION);
    std::cout << "PTS(s)" << std::endl;

    std::map<unsigned int, double>::iterator ii;
    for (ii=m_ptsMap.begin(); ii!=m_ptsMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(DUMP_PRECISION);
        std::cout << (*ii).second << std::endl;
    }

    std::cout << std::endl;
}

void timestamp::DumpDts()
{
    // test if pcr found
    if (m_dtsMap.empty()) return;

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(DUMP_PRECISION);
    std::cout << "DTS(s)" << std::endl;

    std::map<unsigned int, double>::iterator ii;
    for (ii=m_dtsMap.begin(); ii!=m_dtsMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(DUMP_PRECISION);
        std::cout << (*ii).second << std::endl;
    }
}

void timestamp::DumpDelta()
{
    std::map<unsigned int, double> *delta_map;

    if (m_pidpcr != TIMESTAMP_NO_PID) {
        delta_map = &m_pcrMap;
    }
    else if(m_pidpts != TIMESTAMP_NO_PID) {
        delta_map = &m_ptsMap;
    }
    else if(m_piddts != TIMESTAMP_NO_PID) {
        delta_map = &m_dtsMap;
    }
    else
        return;

    // test if there are some data
    if (delta_map->empty()) return;

    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(DUMP_PRECISION);
    std::cout << "Delta (s)" << std::endl;

    double prev_val = 0;
    std::map<unsigned int, double>::iterator ii;
    for (ii=delta_map->begin(); ii!=delta_map->end(); ++ii)
    {
        if (prev_val != 0) {
            std::cout.width(10);
            std::cout << (*ii).first;
            std::cout.width(DUMP_PRECISION);
            std::cout << (*ii).second - prev_val << std::endl;
        }
        prev_val = (*ii).second;
    }
}

void timestamp::DumpJitterPcr()
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);
    std::cout.width(10);
    std::cout << "Index";
    std::cout.width(DUMP_PRECISION);
    std::cout << "Jitter PCR(µs)" << std::endl;

    double bitrate = getBitrate();
    double prev_pcr = 0;
    unsigned int prev_index = 0;

    std::map<unsigned int, double>::iterator ii;
    for (ii=m_pcrMap.begin(); ii!=m_pcrMap.end(); ++ii)
    {
        if (prev_pcr && prev_index)
        {
            // estimate pcr is compute using 27000000 clock
            double estimate_pcr = prev_pcr;
            double offset = ((*ii).first - prev_index) * 188 * 8;
            estimate_pcr += offset/bitrate;

            // jitter in µs
            double jitter = (*ii).second*1000000 - estimate_pcr*1000000;

            std::cout.width(10);
            std::cout << (*ii).first;
            std::cout.width(DUMP_PRECISION);
            std::cout << jitter << std::endl;
        }

        prev_index = (*ii).first;
        prev_pcr = (*ii).second;
    }
}

void timestamp::DumpDiff()
{
    double bitrate = getBitrate();
    std::map<unsigned int, double> *diff_map1, *diff_map2;

    std::cout.flags ( std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout << std::endl;
    std::cout.width(20);
    std::cout << "Buffer Level(s)";
    std::cout.width(20);
    std::cout << "(Kb)" << std::endl;

    if (m_pidpcr != TIMESTAMP_NO_PID) {
        diff_map1 = &m_pcrMap;

        // dts - pcr
        if(m_piddts != TIMESTAMP_NO_PID) {
            diff_map2 = &m_dtsMap;
        }
        // pts - pcr
        else if(m_pidpts != TIMESTAMP_NO_PID) {
            diff_map2 = &m_ptsMap;
        }
        else return;
    }
    else if(m_piddts != TIMESTAMP_NO_PID) {
        diff_map1 = &m_dtsMap;

        // pts - dts
        if(m_pidpts != TIMESTAMP_NO_PID) {
            diff_map2 = &m_ptsMap;
        }
        else return;
    }
    else return;


    std::map<unsigned int, double>::iterator ii;
    unsigned int prev_index = 0;
    double prev_value = 0;
    for (ii = diff_map1->begin(); ii != diff_map1->end(); ++ii) {

        if (prev_index != 0) {
            for (unsigned int index = prev_index; index < (*ii).first; ++index) {
                if (diff_map2->find(index) != diff_map2->end()) {

                    // diff between different timestamp
                    // should be on the same packet or higher
                    double diff = diff_map2->find(index)->second - prev_value;

                    std::cout.width(20);
                    std::cout << diff;
                    std::cout.width(20);
                    std::cout << diff*bitrate/1024;
                    std::cout << std::endl;
                 }
            }
        }

        prev_index = (*ii).first;
        prev_value = (*ii).second;
    }
}
