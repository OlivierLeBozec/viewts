#ifndef THREAD_H
#define THREAD_H

#include <QDebug>
#include <QRunnable>
#include <QtCharts/QLineSeries>

#include "chart.h"
#include "../src/timestamp.h"

QT_CHARTS_USE_NAMESPACE

class timeStampWorker : public QObject, public QRunnable
{
    Q_OBJECT

protected:
    timestamp* m_timestamp;
    Chart* m_chart;

    QLineSeries* m_Series;

    unsigned int m_fileSize;
    unsigned int m_nbProgress;
    unsigned int m_progress;

    // number of packet read by library before run() returns
    const unsigned int m_WindowPacket = 5000;

public:
    timeStampWorker(std::ifstream *tsFile, Chart *chart);
    ~timeStampWorker();

    void updateChart();
    void hideChart();
    void updateProgress();
    void serializeSerie();

    virtual void run() = 0;

signals:
     void finished();
     void updated(int);
};


////////////////////
// PCR worker
class pcrWorker : public timeStampWorker
{
public:
    pcrWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};

// PCR Delta worker
class pcrDeltaWorker : public timeStampWorker
{
public:
    pcrDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};

// PCR Jitter worker
class pcrJitterWorker : public timeStampWorker
{
public:
    pcrJitterWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};


////////////////////
// PTS worker
class ptsWorker : public timeStampWorker
{
public:
    ptsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};

// PTS Delta worker
class ptsDeltaWorker : public timeStampWorker
{
public:
    ptsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};


////////////////////
// DTS worker
class dtsWorker : public timeStampWorker
{
public:
    dtsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};

// DTS delta worker
class dtsDeltaWorker : public timeStampWorker
{
public:
    dtsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart);
    virtual void run();
};

////////////////
// Diff PCR PTS worker
class diffPcrPtsWorker : public timeStampWorker
{
public:
    diffPcrPtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    virtual void run();
};

// Diff PCR DTS worker
class diffPcrDtsWorker : public timeStampWorker
{
public:
    diffPcrDtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    virtual void run();
};

// Diff PTS DTS worker
class diffPtsDtsWorker : public timeStampWorker
{
public:
    diffPtsDtsWorker(std::ifstream *tsFile, unsigned int pidPts, unsigned int pidDts, Chart *chart);
    virtual void run();
};

#endif // THREAD_H
