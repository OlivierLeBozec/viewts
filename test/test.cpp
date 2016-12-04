#include "../src/timestamp.h"
#include "../src/pidmap.h"
#include "gtest/gtest.h"


TEST(pid, check_PCR_pid_on_audio)
{
    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);

    pidmap pm(tsFile);
    std::vector<unsigned int> pcrPid;
    pm.getPcrPid(pcrPid);

    EXPECT_EQ(pcrPid[0], 305);
}

TEST(pcr, check_PCR_values_on_audio)
{
    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);

    timestamp ts(tsFile, 305);
    EXPECT_DOUBLE_EQ(ts.getPcr(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(5), 5815.190934000000);
    EXPECT_DOUBLE_EQ(ts.getPcr(99), 5817.272571407408);
}

TEST(pcr, check_PCR_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, 220);
    EXPECT_DOUBLE_EQ(ts.getPcr(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(39), 54395.234549296300);
    EXPECT_DOUBLE_EQ(ts.getPcr(9970), 54398.382614740738);
}

TEST(pts, check_PTS_values_on_audio)
{
    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);

    timestamp ts(tsFile, TIMESTAMP_NO_PID, 305);
    EXPECT_DOUBLE_EQ(ts.getPts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPts(2), 5820.125477777778);
    EXPECT_DOUBLE_EQ(ts.getPts(92), 5822.141477777778);
}

TEST(pts, check_PTS_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, TIMESTAMP_NO_PID, 220);
    EXPECT_DOUBLE_EQ(ts.getPts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPts(90), 54395.691555555553);
    EXPECT_DOUBLE_EQ(ts.getPts(9698), 54398.771555555555);
}

TEST(dts, check_DTS_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, 220);
    EXPECT_DOUBLE_EQ(ts.getDts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getDts(263), 54395.771555555555);
    EXPECT_DOUBLE_EQ(ts.getDts(9698), 54398.651555555552);
}

TEST(delta, check_deltaPCR_values_on_audio)
{
    unsigned int index;
    double delta;

    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);
    timestamp ts(tsFile, 305);

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 10);
    EXPECT_DOUBLE_EQ(delta, 0.10204555555537809);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 99);
    EXPECT_DOUBLE_EQ(delta, 0.10204555555537809);
}

TEST(delta, check_deltaPTS_values_on_video_dvbSub)
{
    unsigned int index;
    double delta;

    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);
    timestamp ts(tsFile, TIMESTAMP_NO_PID, 220);

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 176);
    EXPECT_DOUBLE_EQ(delta, 0.040000000000873115);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(delta, 0.1599999999962165);
}

TEST(delta, check_deltaDTS_values_on_video_dvbSub)
{
    unsigned int index;
    double delta;

    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);
    timestamp ts(tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, 220);

    // first value
    EXPECT_EQ(ts.getNextDelta(index, delta), true);
    EXPECT_EQ(index, 722);
    EXPECT_DOUBLE_EQ(delta, 0.12000000000261934);

    // last value
    while(ts.getNextDelta(index, delta) == true);
    EXPECT_EQ(index, 9698);
    EXPECT_DOUBLE_EQ(delta, 0.11999999999534339);
}

TEST(diff, check_diffPcrPts_values_on_audio)
{
    unsigned int index;
    double diff;

    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);
    timestamp ts(tsFile, 305, 305);

    // first value
    EXPECT_EQ(ts.getNextDiff(index, diff), true);
    EXPECT_EQ(index, 10);
    EXPECT_DOUBLE_EQ(diff, 4.9924982222219114);

    // last value
    while (ts.getNextDiff(index, diff) == true);
    EXPECT_EQ(index, 89);
    EXPECT_DOUBLE_EQ(diff, 5.0089885925926865);
}

TEST(diff, check_diffPtsDts_values_on_video_dvbSub)
{
    unsigned int index;
    double diff;

    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);
    timestamp ts(tsFile, TIMESTAMP_NO_PID, 220, 220);

    // first value
    EXPECT_EQ(ts.getNextDiff(index, diff), true);
    EXPECT_EQ(index, 263);
    EXPECT_DOUBLE_EQ(diff, 0.12000000000261934);

    // last value
    while (ts.getNextDiff(index, diff) == true);
    EXPECT_EQ(index, 9323);
    EXPECT_DOUBLE_EQ(diff, 0.11999999999534339);
}
