#include "pidmap.h"
#include "packet.h"
#include "pes.h"

pidmap::pidmap(std::ifstream& fileIn)
{
    // fill the output file with PCR
    unsigned char data[188];

    // TODO: find first start byte (repeated 0x47 every 188)
    // TODO: thread

    // loop on packet
    fileIn.clear();
    fileIn.seekg(0);    // TODO
    while (fileIn.read((char*)data, 188))
    {
        // create packet from buffer
        packet packet(data);
        unsigned int pid = packet.getPid();

        // update pid list
        m_pidVec.push_back(pid);

        // update pattern
        if (m_pidMap[pid].nb_packet == 0)
            m_pidMap[pid].pattern = (char)('0' + m_pidMap.size() - 1);

        // other values
        m_pidMap[pid].nb_packet++;
        m_pidMap[pid].has_pcr |= packet.hasPcr();

        if (packet.hasPes()) {
            // create pes from buffer
            pes pes(data + packet.getPesOffset());
            m_pidMap[pid].has_pts |= pes.hasPts();
            m_pidMap[pid].has_dts |= pes.hasDts();
        }
    }

    // calc percentage
    std::map<unsigned int, pidinfo>::iterator ii;
    for (ii=m_pidMap.begin(); ii!=m_pidMap.end(); ++ii)
    {
        (*ii).second.percent = static_cast<float>((*ii).second.nb_packet)*100;
        (*ii).second.percent /= m_pidVec.size();
    }
}

pidmap::~pidmap()
{
    if (!m_pidMap.empty()) {
        m_pidMap.clear();
    }
    if (!m_pidVec.empty()) {
        m_pidVec.clear();
    }
}

void pidmap::getPcrPid(std::vector<unsigned int>&  pidVector) {

    std::map<unsigned int, pidinfo>::iterator ii;
    for (ii=m_pidMap.begin(); ii!=m_pidMap.end(); ++ii)
    {
        if((*ii).second.has_pcr)
            pidVector.push_back((*ii).first);
    }
}

void pidmap::getPtsPid(std::vector<unsigned int>&  pidVector) {

    std::map<unsigned int, pidinfo>::iterator ii;
    for (ii=m_pidMap.begin(); ii!=m_pidMap.end(); ++ii)
    {
        if((*ii).second.has_pts)
            pidVector.push_back((*ii).first);
    }
}

void pidmap::getDtsPid(std::vector<unsigned int>&  pidVector) {

    std::map<unsigned int, pidinfo>::iterator ii;
    for (ii=m_pidMap.begin(); ii!=m_pidMap.end(); ++ii)
    {
        if((*ii).second.has_dts)
            pidVector.push_back((*ii).first);
    }
}

void pidmap::OutPid() {

    std::cout.flags ( std::ios_base::fixed | std::ios::left | std::ios_base::showbase);
    std::cout.precision(3);

    std::cout << std::endl;
    std::cout.width(10);
    std::cout << "PID";
    std::cout.width(10);
    std::cout << "PID hex";
    std::cout.width(10);
    std::cout << "Nb packet";
    std::cout.width(10);
    std::cout << "Percent";
    std::cout.width(10);
    std::cout << "has Pcr";
    std::cout.width(10);
    std::cout << "has Pts";
    std::cout.width(10);
    std::cout << "has Dts";
    std::cout.width(10);
    std::cout << "Pattern" << std::endl;

    std::map<unsigned int, pidinfo>::iterator ii;
    for (ii=m_pidMap.begin(); ii!=m_pidMap.end(); ++ii)
    {
        std::cout.width(10);
        std::cout << (*ii).first;
        std::cout.width(10);
        std::cout << std::hex << (*ii).first << std::dec;
        std::cout.width(10);
        std::cout << (*ii).second.nb_packet;
        std::cout.width(10);
        std::cout << (*ii).second.percent;
        std::cout.width(10);
        if((*ii).second.has_pcr) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        if((*ii).second.has_pts) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        if((*ii).second.has_dts) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        std::cout << (*ii).second.pattern << std::endl;
    }
}

void pidmap::OutMap() {

    std::cout << "Map of PIDs" << std::endl;

    std::vector<unsigned int>::iterator ii;
    for (ii=m_pidVec.begin(); ii!=m_pidVec.end(); ++ii)
    {
        std::cout << m_pidMap[*ii].pattern;
    }

    std::cout << std::endl;
}
