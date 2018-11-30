#include <iostream>
#include <string>
#include <fstream>

#include "pidmap.h"

#define VERSION "1.0"

void Usage(char *pName) {
    std::cout << "NAME" << std::endl;
    std::cout << "   " << pName << " - list mpeg ts content" << std::endl;
    std::cout << "   Version " << VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   " << pName << " FILE [-pidlist -pidmap]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION" << std::endl;
    std::cout << "   List mpeg information about the FILE. Without using PSI-SI tables" << std::endl;
    std::cout << std::endl;
    std::cout << "    -pidlist" << std::endl;
    std::cout << "          list of the pid in the file" << std::endl;
    std::cout << std::endl;
    std::cout << "    -pidmap" << std::endl;
    std::cout << "          map of the pid in the file" << std::endl;
}

void dumpPidmap(pidmap& pm)
{
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

    unsigned int pid;
    pidinfo pidInfo;
    while (pm.GetNextPidInfo(pid, pidInfo) == true)
    {
        std::cout.width(10);
        std::cout << pid;
        std::cout.width(10);
        std::cout << std::hex << pid << std::dec;
        std::cout.width(10);
        std::cout << pidInfo.nb_packet;
        std::cout.width(10);
        std::cout << pidInfo.percent;
        std::cout.width(10);
        if(pidInfo.has_pcr) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        if(pidInfo.has_pts) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        if(pidInfo.has_dts) std::cout << "Yes"; else std::cout << "No";
        std::cout.width(10);
        std::cout << pidInfo.pattern << std::endl;
    }
}

void dumpMap(pidmap& pm)
{
    std::cout << "Map of PIDs" << std::endl;

    unsigned char pattern;
    while (pm.GetNextPattern(pattern) == true)
    {
        std::cout << pattern;
    }

    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    // help
    if (argc == 1)
    {
        Usage(argv[0]);
        return 0;
    }

    // check options
    std::string StrPidList("-pidlist"), StrPidMap ("-pidmap");
    bool pidlist = false, pidm = false;

    for (int i=2; i<argc; ++i)
    {
        std::string StrOption(argv[i]);
        if (StrOption == StrPidList) pidlist = true;
        else if (StrOption == StrPidMap) pidm = true;
    }

    // display pid info
    if (pidlist || pidm){
        std::string Filename = argv[1];
        pidmap pm(&Filename);

        while (pm.run() == true);
        if (pidlist)    dumpPidmap(pm);
        if (pidm)       dumpMap(pm);
    }

    return 0;
}
