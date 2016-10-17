#include <iostream>
#include <assert.h>
#include <string>
#include <fstream>

#include <stdlib.h> // atoi()

#include "pidmap.h"

void Usage() {
    std::cout << "NAME" << std::endl;
    std::cout << "   tsmap - list mpeg ts content" << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   tsmap FILE [-pidlist -pidmap]" << std::endl;
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

int main(int argc, char** argv)
{
    // help
    if (argc == 1)
    {
        Usage();
        return 0;
    }

    // check filename
    std::ifstream tsFile(argv[1], std::ios::binary);
    if (!tsFile.is_open())
    {
        std::cerr << "Failed to open file " << argv[1] << std::endl;
        return -1;
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
        pidmap pm(tsFile);
        if (pidlist)    pm.OutPid();
        if (pidm)       pm.OutMap();
    }

    // close
    tsFile.close();

    return 0;
}
