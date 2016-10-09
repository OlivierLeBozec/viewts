#include <iostream>
#include <assert.h>
#include <string>
#include <fstream>

#include <stdlib.h> // atoi()

#include "timestamp.h"

void Usage() {
    std::cout << "NAME" << std::endl;
    std::cout << "   tscheck - check timestamp" << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   tscheck FILE -pid <PID> [...]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION" << std::endl;
    std::cout << "   Display specified timestamp" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pid <PID>" << std::endl;
    std::cout << "          pid to check for" << std::endl;
    std::cout << std::endl;
    std::cout << "   Following commands are available only if the PID is specified" << std::endl;
    std::cout << std::endl;
    std::cout << "    -dur" << std::endl;
    std::cout << "          get duration of the stream" << std::endl;
    std::cout << std::endl;
    std::cout << "   -rate" << std::endl;
    std::cout << "          get  bitrate of the stream" << std::endl;
    std::cout << std::endl;
    std::cout << "   -delta" << std::endl;
    std::cout << "          diff between consecutive pcr" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pcr" << std::endl;
    std::cout << "          display pcr" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pts" << std::endl;
    std::cout << "          display pts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -dts" << std::endl;
    std::cout << "          display dts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -jitter" << std::endl;
    std::cout << "          display pcr jitter" << std::endl;
    std::cout << std::endl;
    std::cout << "   -lvl" << std::endl;
    std::cout << "        display input buffer level" << std::endl;
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
    std::string StrBitrate("-rate"), StrDuration("-dur"), StrPid("-pid"), StrPcr("-pcr"), StrPts("-pts"), StrDts("-dts"),
            StrDelta("-delta"), StrJitter("-jitter"), StrLevel("-lvl");

    bool rate = false, dur = false, pcr = false, pts = false, dts = false, delta = false, jitter = false, lvl = false;
    unsigned int pid = 0xFFFF;

    for (int i=2; i<argc; ++i)
    {
        std::string StrOption(argv[i]);
        if (StrOption == StrBitrate) rate = true;
        else if (StrOption == StrDuration) dur = true;
        else if (StrOption == StrPcr) pcr = true;
        else if (StrOption == StrPts) pts = true;
        else if (StrOption == StrDts) dts = true;
        else if (StrOption == StrPid) pid = atoi(argv[++i]);
        else if (StrOption == StrDelta) delta = true;
        else if (StrOption == StrJitter) jitter = true;
        else if (StrOption == StrLevel) lvl = true;
    }

    if ((rate || dur || pcr || pts || dts || delta || jitter) && pid == 0xFFFF) {
        Usage();
        goto end;
    }

    // display timestamp
    if (rate || dur || pcr || pts || dts || delta || jitter || lvl){
        timestamp ts(tsFile, pid);
        if (dur)    ts.OutDuration();
        if (rate)   ts.OutBitrate();
        if (pcr)    ts.OutPcr();
        if (pts)    ts.OutPts();
        if (dts)    ts.OutDts();
        if (delta)  ts.OutDeltaPcr();
        if (jitter) ts.OutJitterPcr();
        if (lvl)    ts.OutBufferLevel();
    }

end:
    // close
    tsFile.close();

    return 0;
}
