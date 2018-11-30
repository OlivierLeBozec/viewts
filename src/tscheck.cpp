#include <iostream>
#include <string>
#include <fstream>

#include <stdlib.h> // atoi()

#include "timestamp.h"

#define VERSION "1.0"
#define DUMP_PRECISION 12

void DumpDuration(timestamp& ts)
{
    double duration = ts.getDuration();

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

void DumpBitrate(timestamp& ts)
{
    double bitrate = ts.getGlobalBitrate();

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

void DumpPcr(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "PCR(s)" << std::endl;

    unsigned int index;
    double pcr;
    while (ts.getNextPcr(index, pcr) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << pcr << std::endl;
    }

    std::cout << std::endl;
}

void DumpPts(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "PTS(s)" << std::endl;

    unsigned int index;
    double pts;
    while (ts.getNextPts(index, pts) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << pts << std::endl;
    }

    std::cout << std::endl;
}

void DumpDts(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "DTS(s)" << std::endl;

    unsigned int index;
    double dts;
    while (ts.getNextDts(index, dts) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << dts << std::endl;
    }
}

void DumpDelta(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "Delta (s)" << std::endl;

    unsigned int index;
    double delta;
    while (ts.getNextDelta(index, delta) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << delta << std::endl;
    }
}

void DumpJitterPcr(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "Jitter PCR (s)" << std::endl;

    unsigned int index;
    double jitter;
    while (ts.getNextJitterPcr(index, jitter) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << jitter << std::endl;
    }
}

void DumpDiff(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "timestamp diff (s)" << std::endl;

    unsigned int index;
    double diff;
    while (ts.getNextDiff(index, diff) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << diff << std::endl;
    }
}

void DumpLocalBitrate(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);
    std::cout.precision(DUMP_PRECISION);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "local bitrate (b/s)" << std::endl;

    unsigned int index;
    double bitrate;
    while (ts.getNextBitrate(index, bitrate) == true)
    {
        std::cout.width(10);
        std::cout << index;
        std::cout.width(DUMP_PRECISION);
        std::cout << bitrate << std::endl;
    }
}

void DumpLevel(timestamp& ts)
{
    std::cout.flags (std::ios_base::fixed | std::ios::left);

    std::cout.width(10);
    std::cout << "Index";
    std::cout << "level (byte)" << std::endl;

    unsigned int index;
    double level;
    while (ts.getNextLevel(index, level) == true)
    {
        std::cout.width(10);
        std::cout << index << (int)level << std::endl;
    }
}

void Usage(char *pName) {
    std::cout << "NAME" << std::endl;
    std::cout << "   " << pName << " - check timestamp" << std::endl;
    std::cout << "   Version " << VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   " << pName << " FILE -pid <PID> [...]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION" << std::endl;
    std::cout << "   Display specified timestamp operation" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pidpcr <PID>" << std::endl;
    std::cout << "          set pcr pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   -pidpts <PID>" << std::endl;
    std::cout << "          set pts pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   -piddts <PID>" << std::endl;
    std::cout << "          set dts pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   Following commands are available only at least one PID is specified" << std::endl;
    std::cout << std::endl;
    std::cout << "   -dump" << std::endl;
    std::cout << "          dump timestamp" << std::endl;
    std::cout << std::endl;
    std::cout << "   -dur" << std::endl;
    std::cout << "          get duration of the stream" << std::endl;
    std::cout << std::endl;
    std::cout << "   -globalrate" << std::endl;
    std::cout << "          get global bitrate of pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   -localrate" << std::endl;
    std::cout << "          get local bitrate of pid" << std::endl;
    std::cout << std::endl;
    std::cout << "   -delta" << std::endl;
    std::cout << "          diff between same consecutive timestamp for pcr, pts or dts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -jitter" << std::endl;
    std::cout << "          display jitter for pcr" << std::endl;
    std::cout << std::endl;
    std::cout << "   -diff" << std::endl;
    std::cout << "          diff between 2 timestamps : pts - pcr, dts - pts or pts - dts" << std::endl;
    std::cout << std::endl;
    std::cout << "   -level" << std::endl;
    std::cout << "          input buffer level of pid with pes" << std::endl;
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
    std::string StrDump("-dump"), StrBitrate("-rate"), StrDuration("-dur"), StrPcr("-pidpcr"), StrPts("-pidpts"), StrDts("-piddts"),
            StrDelta("-delta"), StrJitter("-jitter"), StrDiff("-diff"), StrLocalBitrate("-localrate"), StrLevel("-level");

    unsigned int  pidpcr = TIMESTAMP_NO_PID, pidpts = TIMESTAMP_NO_PID, piddts = TIMESTAMP_NO_PID;
    bool dump = false, rate = false, dur = false, delta = false, jitter = false, diff = false, localbitrate = false, level = false;

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
        else if (StrOption == StrLocalBitrate) localbitrate = true;
        else if (StrOption == StrLevel) level = true;
    }

    if (pidpcr == TIMESTAMP_NO_PID && pidpts == TIMESTAMP_NO_PID && piddts == TIMESTAMP_NO_PID)
    {
        Usage(argv[0]);
        return 0;
    }

    // display timestamp
    if (dump || rate || dur || delta || jitter || diff || localbitrate || level){
        std::string Filename = std::string(argv[1]);
        timestamp ts(&Filename, pidpcr, pidpts, piddts);
        ts.run();
        if (dump && pidpcr != TIMESTAMP_NO_PID) DumpPcr(ts);
        if (dump && pidpts != TIMESTAMP_NO_PID) DumpPts(ts);
        if (dump && piddts != TIMESTAMP_NO_PID) DumpDts(ts);
        if (dur)    DumpDuration(ts);
        if (rate)   DumpBitrate(ts);
        if (delta)  DumpDelta(ts);
        if (jitter && pidpcr != TIMESTAMP_NO_PID) DumpJitterPcr(ts);
        if (diff)   DumpDiff(ts);
        if (localbitrate && pidpcr != TIMESTAMP_NO_PID) DumpLocalBitrate(ts);
        if (level && (pidpts != TIMESTAMP_NO_PID || piddts != TIMESTAMP_NO_PID)) DumpLevel(ts);
    }

    return 0;
}
