#include "../src/timestamp.h"
#include "../src/pidmap.h"
#include "gtest/gtest.h"


TEST(pid, check_pid_on_video_dvbSub)
{
    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    pidmap pm(Filename);

    // check first 10000 packet
    EXPECT_EQ(pm.run(10000), true);

    std::vector<unsigned int> Pid;
    pm.getPcrPid(Pid);
    EXPECT_EQ(Pid[0], 220);

    Pid.clear();
    pm.getPtsPid(Pid);
    EXPECT_EQ(Pid[0], 220);
    EXPECT_EQ(Pid[1], 230);
    EXPECT_EQ(Pid[2], 231);

    Pid.clear();
    pm.getDtsPid(Pid);
    EXPECT_EQ(Pid[0], 220);

    delete Filename;
}

TEST(pcr, check_PCR_values_on_audio)
{
    unsigned int index = 0;
    double pcr;

    std::string *Filename = new std::string("../../ts/audio.ts");
    timestamp ts(Filename, 305);

    // run leave when completed
    EXPECT_EQ(ts.run(), true);

    // first value
    EXPECT_EQ(ts.getNextPcr(index, pcr), true);
    EXPECT_EQ(index, 5);
    EXPECT_DOUBLE_EQ(pcr, 5815.190934000000);

    // last value
    while(ts.getNextPcr(index, pcr) == true);
    EXPECT_EQ(index, 99);
    EXPECT_DOUBLE_EQ(pcr, 5817.272571407408);

    EXPECT_EQ(ts.getNextPcr(index, pcr), false);

    delete Filename;
}

TEST(pcr, check_PCR_values_on_video_dvbSub)
{
    unsigned int index = 0;
    double pcr;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, 220);

    // run leave every 1000 packet
    while(ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextPcr(index, pcr), true);
    EXPECT_EQ(index, 39);
    EXPECT_DOUBLE_EQ(pcr, 54395.234549296300);

    // last value
    while(ts.getNextPcr(index, pcr) == true);
    EXPECT_EQ(index, 9970);
    EXPECT_DOUBLE_EQ(pcr, 54398.382614740738);

    EXPECT_EQ(ts.getNextPcr(index, pcr), false);

    delete Filename;
}

TEST(pts, check_PTS_values_on_audio)
{
    unsigned int index = 0;
    double pts;

    std::string *Filename = new std::string("../../ts/audio.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, 305);

    EXPECT_EQ(ts.run(), true);

    // first value
    EXPECT_EQ(ts.getNextPts(index, pts), true);
    EXPECT_EQ(index, 2);
    EXPECT_DOUBLE_EQ(pts, 5820.125477777778);

    // last value
    while(ts.getNextPts(index, pts) == true);
    EXPECT_EQ(index, 92);
    EXPECT_DOUBLE_EQ(pts, 5822.141477777778);

    EXPECT_EQ(ts.getNextPts(index, pts), false);

    delete Filename;
}

TEST(pts, check_PTS_values_on_video_dvbSub)
{
    unsigned int index = 0;
    double pts;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, 220);

    // run leave every 1000 packet
    while(ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextPts(index, pts), true);
    EXPECT_EQ(index, 90);
    EXPECT_DOUBLE_EQ(pts, 54395.691555555553);

    // last value
    while(ts.getNextPts(index, pts) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(pts, 54398.771555555555);

    EXPECT_EQ(ts.getNextPts(index, pts), false);

    delete Filename;
}

TEST(dts, check_DTS_values_on_video_dvbSub)
{
    unsigned int index = 0;
    double dts;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, 220);

    // run leave every 1000 packet
    while(ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextDts(index, dts), true);
    EXPECT_EQ(index, 263);
    EXPECT_DOUBLE_EQ(dts, 54395.771555555555);

    // last value
    while(ts.getNextDts(index, dts) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(dts, 54398.651555555552);

    EXPECT_EQ(ts.getNextDts(index, dts), false);

    delete Filename;
}

TEST(delta, check_deltaPCR_values_on_audio)
{
    unsigned int index;
    double delta;

    std::string *Filename = new std::string("../../ts/audio.ts");
    timestamp ts(Filename, 305);
    ts.run();

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 10);
    EXPECT_DOUBLE_EQ(delta, 0.10204555555537809);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 99);
    EXPECT_DOUBLE_EQ(delta, 0.10204555555537809);

    EXPECT_EQ(ts.getNextDelta(index, delta), false);

    delete Filename;
}

TEST(delta, check_deltaPTS_values_on_video_dvbSub)
{
    unsigned int index;
    double delta;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, 220);

    // run leave every 1000 packet
    while(ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 176);
    EXPECT_DOUBLE_EQ(delta, 0.040000000000873115);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(delta, 0.1599999999962165);

    EXPECT_EQ(ts.getNextDelta(index, delta), false);

    delete Filename;
}

TEST(delta, check_deltaDTS_values_on_video_dvbSub)
{
    unsigned int index;
    double delta;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, 220);

    // run leave every 1000 packet
    while(ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 722);
    EXPECT_DOUBLE_EQ(delta, 0.12000000000261934);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(delta, 0.11999999999534339);

    EXPECT_EQ(ts.getNextDelta(index, delta), false);

    delete Filename;
}

TEST(diff, check_diffPcrPts_values_on_audio)
{
    unsigned int index;
    double diff;

    std::string *Filename = new std::string("../../ts/audio.ts");
    timestamp ts(Filename, 305, 305);
    EXPECT_EQ(ts.run(), true);

    // first value
    EXPECT_EQ(ts.getNextDiff(index, diff), true);
    EXPECT_EQ(index, 10);
    EXPECT_DOUBLE_EQ(diff, 4.9924982222219114);

    // last value
    while (ts.getNextDiff(index, diff) == true);
    EXPECT_EQ(index, 89);
    EXPECT_DOUBLE_EQ(diff, 5.0089885925926865);

    delete Filename;
}

TEST(diff, check_diffPtsDts_values_on_video_dvbSub)
{
    unsigned int index;
    double diff;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, TIMESTAMP_NO_PID, 220, 220);
    while (ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextDiff(index, diff), true);
    EXPECT_EQ(index, 263);
    EXPECT_DOUBLE_EQ(diff, 0.12000000000261934);

    // last value
    while (ts.getNextDiff(index, diff) == true);
    EXPECT_EQ(index, 9323);
    EXPECT_DOUBLE_EQ(diff, 0.11999999999534339);

    EXPECT_EQ(ts.getNextDiff(index, diff), false);

    delete Filename;
}

TEST(jitter, check_jitterPcr_values_on_video_dvbSub)
{
    unsigned int index;
    double jitter;

    std::string *Filename = new std::string("../../ts/video_dvbSub.ts");
    timestamp ts(Filename, 220);
    while (ts.run(1000) == true);

    // first value
    EXPECT_EQ(ts.getNextJitterPcr(index, jitter), true);
    EXPECT_EQ(index, 56);
    EXPECT_DOUBLE_EQ(jitter, 0.023075623888871633);

    // last value
    while (ts.getNextJitterPcr(index, jitter) == true);
    EXPECT_EQ(index, 9970);
    EXPECT_DOUBLE_EQ(jitter, 0.022117092819826212);

    EXPECT_EQ(ts.getNextJitterPcr(index, jitter), false);

    delete Filename;
}
