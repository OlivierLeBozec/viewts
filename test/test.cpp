#include "../src/timestamp.h"
#include "gtest/gtest.h"

TEST(pcr, check_PCR_values_on_audio)
{
    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);

    timestamp ts(tsFile, 305, 0, 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(5), 5815.190934000000);
    EXPECT_DOUBLE_EQ(ts.getPcr(99), 5817.272571407408);
}

TEST(pcr, check_PCR_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, 220, 0, 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPcr(39), 54395.234549296300);
    EXPECT_DOUBLE_EQ(ts.getPcr(9970), 54398.382614740738);
}

TEST(pts, check_PTS_values_on_audio)
{
    std::ifstream tsFile("../../ts/audio.ts", std::ios::binary);

    timestamp ts(tsFile, 0, 305, 0);
    EXPECT_DOUBLE_EQ(ts.getPts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPts(2), 5820.125477777778);
    EXPECT_DOUBLE_EQ(ts.getPts(92), 5822.141477777778);
}

TEST(pts, check_PTS_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, 0, 220, 0);
    EXPECT_DOUBLE_EQ(ts.getPts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getPts(90), 54395.691555555553);
    EXPECT_DOUBLE_EQ(ts.getPts(9698), 54398.771555555555);
}

TEST(dts, check_DTS_values_on_video_dvbSub)
{
    std::ifstream tsFile("../../ts/video_dvbSub.ts", std::ios::binary);

    timestamp ts(tsFile, 0, 0, 220);
    EXPECT_DOUBLE_EQ(ts.getDts(0), 0);
    EXPECT_DOUBLE_EQ(ts.getDts(263), 54395.771555555555);
    EXPECT_DOUBLE_EQ(ts.getDts(9698), 54398.651555555552);
}

