#ifndef WORKERS_H
#define WORKERS_H

#include <QDebug>
#include <QThread>
#include <QRunnable>
#include <QtCharts/QLineSeries>

#include "chart.h"
#include "../src/timestamp.h"

QT_CHARTS_USE_NAMESPACE

class infoWorker : public QObject, public QRunnable
{
    Q_OBJECT

    // run function from QRunnable
    void run();

protected:
    // retrieve the time stamps
    timestamp* m_timestamp;

public:
    infoWorker(std::string &tsFile, unsigned int pid);
    ~infoWorker();

    // true if the thread is running
    bool m_isRunning;

    // manipulate the series
    double getGlobalBitrate();
    double getGlobalDuration();

signals:
    void finished();
};

class timeStampWorker : public QObject, public QRunnable
{
    Q_OBJECT

    // used to compute competion rate
    unsigned int m_nbProgress;
    unsigned long long m_fileSize;

protected:
    // number of packet read by library before run() returns
    const unsigned int m_WindowPacket = 10000;

    // retrieve the time stamps
    timestamp* m_timestamp;

    // chart for drawing
    Chart* m_chart;

    // axis
    bool m_isTimeXaxis;

    // series of data to draw
    QLineSeries* m_Series;

    // function used in thread
    virtual bool getData(unsigned int& index, double& val) = 0;

    // update the competion rate
    virtual void updateProgress();

public:
    timeStampWorker(std::string &tsFile, Chart *chart);
    ~timeStampWorker();

    QLineSeries* getSeries();

    void SetTimeAxis (bool isTimeXaxis) {
        m_isTimeXaxis = isTimeXaxis;
    }

    // true if the thread is running
    bool m_isRunning;

    // true if the thread is aborting
    bool m_isAborting;

    // manipulate the series
    virtual void showSeries();
    virtual void hideSeries();

    // write the serie of points in file
    virtual void serializeSeries(std::ofstream *outFile);

    // abort the thread
    virtual void abort();

    // run function from QRunnable
    virtual void run();

signals:
     void finished();
     void updated(int);
};

////////////////////
// PCR worker
class pcrWorker : public timeStampWorker
{
public:
    pcrWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextPcr(index, val);
    }
};

class pcrDeltaWorker : public timeStampWorker
{
public:
    pcrDeltaWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDelta(index, val);
    }
};

class pcrJitterWorker : public timeStampWorker
{
public:
    pcrJitterWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextJitterPcr(index, val);
    }
};

class pcrBitrateWorker : public timeStampWorker
{
public:
    pcrBitrateWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextBitrate(index, val);
    }
};

////////////////////
// PTS worker
class ptsWorker : public timeStampWorker
{
public:
    ptsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextPts(index, val);
    }
};

class ptsDeltaWorker : public timeStampWorker
{
public:
    ptsDeltaWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDelta(index, val);
    }
};


////////////////////
// DTS worker
class dtsWorker : public timeStampWorker
{
public:
    dtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDts(index, val);
    }
};

// DTS delta worker
class dtsDeltaWorker : public timeStampWorker
{
public:
    dtsDeltaWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDelta(index, val);
    }
};

////////////////
// Diff worker
class diffPcrPtsWorker : public timeStampWorker
{
public:
    diffPcrPtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDiff(index, val);
    }
};

class diffPcrDtsWorker : public timeStampWorker
{
public:
    diffPcrDtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDiff(index, val);
    }
};

class diffPtsDtsWorker : public timeStampWorker
{
public:
    diffPtsDtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, unsigned int pidDts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextDiff(index, val);
    }
};

class buffLevelPtsWorker : public timeStampWorker
{
public:
    buffLevelPtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextLevel(index, val);
    }
};

class buffLevelPtsDtsWorker : public timeStampWorker
{
public:
    buffLevelPtsDtsWorker(std::string &tsFile, unsigned int pidPcr, unsigned int pidPts, unsigned int pidDts, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        return m_timestamp->getNextLevel(index, val);
    }
};

#endif // WORKERS_H
