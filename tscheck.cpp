#include <iostream>
#include <string>
#include <fstream>

#include <stdlib.h> // atoi()

#include "timestamp.h"

#define VERSION "1.0"

void Usage(char *pName) {
    std::cout << "NAME" << std::endl;
    std::cout << "   " << pName << " - check timestamp" << std::endl;
    std::cout << std::endl;
    std::cout << "   Version " << VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   " << pName << " FILE -pid <PID> [...]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION" << std::endl;
    std::cout << "   Display specified timestamp operation" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pidpcr <PID>" << std::endl;
    std::cout << "          display pcr" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pidpts <PID>" << std::endl;
    std::cout << "          display pts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -piddts <PID>" << std::endl;
    std::cout << "          display dts" << std::endl;
    std::cout << std::endl;
    std::cout << "   Following commands are available only at least one PID is specified" << std::endl;
    std::cout << std::endl;
    std::cout << "   -dump" << std::endl;
    std::cout << "          dump timestamp" << std::endl;
    std::cout << std::endl;
    std::cout << "   -dur" << std::endl;
    std::cout << "          get duration of the stream" << std::endl;
    std::cout << std::endl;
    std::cout << "   -rate" << std::endl;
    std::cout << "          get bitrate of pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   -delta" << std::endl;
    std::cout << "          diff between same consecutive timestamp for pcr, pts or dts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -jitter" << std::endl;
    std::cout << "          display jitter for pcr" << std::endl;
    std::cout << std::endl;
    std::cout << "   -diff" << std::endl;
    std::cout << "          diff between 2 timestamps : pts - pcr, dts - pts or pts - dts" << std::endl;
}

int main(int argc, char** argv)
{
    // help
    if (argc == 1)
    {
        Usage(argv[0]);
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
    std::string StrDump("-dump"), StrBitrate("-rate"), StrDuration("-dur"), StrPcr("-pidpcr"), StrPts("-pidpts"), StrDts("-piddts"),
            StrDelta("-delta"), StrJitter("-jitter"), StrDiff("-diff");

    unsigned int  pidpcr = TIMESTAMP_NO_PID, pidpts = TIMESTAMP_NO_PID, piddts = TIMESTAMP_NO_PID;
    bool dump = false, rate = false, dur = false, delta = false, jitter = false, diff = false;

    for (int i=2; i<argc; ++i)
    {
        std::string StrOption(argv[i]);
        if (StrOption == StrDump) dump = true;
        else if (StrOption == StrBitrate) rate = true;
        else if (StrOption == StrDuration) dur = true;
        else if (StrOption == StrPcr) pidpcr = atoi(argv[++i]);
        else if (StrOption == StrPts) pidpts = atoi(argv[++i]);
        else if (StrOption == StrDts) piddts = atoi(argv[++i]);
        else if (StrOption == StrDelta) delta = true;
        else if (StrOption == StrJitter) jitter = true;
        else if (StrOption == StrDiff) diff = true;
    }

    if (pidpcr == TIMESTAMP_NO_PID && pidpts == TIMESTAMP_NO_PID && piddts == TIMESTAMP_NO_PID)
    {
        Usage(argv[0]);
        return 0;
    }

    // display timestamp
    if (dump || rate || dur || delta || jitter || diff){
        timestamp ts(tsFile, pidpcr, pidpts, piddts);
        if (dump && pidpcr != TIMESTAMP_NO_PID) ts.DumpPcr();
        if (dump && pidpts != TIMESTAMP_NO_PID) ts.DumpPts();
        if (dump && piddts != TIMESTAMP_NO_PID) ts.DumpDts();
        if (dur)        ts.DumpDuration();
        if (rate)       ts.DumpBitrate();
        if (delta)      ts.DumpDelta();
        if (jitter)     ts.DumpJitterPcr();
        if (diff)       ts.DumpDiff();
    }

    // close
    tsFile.close();

    return 0;
}
