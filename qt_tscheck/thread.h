#ifndef THREAD_H
#define THREAD_H

#include <QDebug>
#include <QRunnable>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "chart.h"
#include "../src/timestamp.h"

QT_CHARTS_USE_NAMESPACE

class timeStampWorker : public QObject, public QRunnable
{
    Q_OBJECT

    Chart* m_chart;

protected:
    timestamp* m_timestamp;

    QLineSeries* m_Series;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    void updateChart();

public:
    timeStampWorker(Chart * chart);
    ~timeStampWorker();

    virtual void run() = 0;

signals:
    void updated();
};


////////////////////
// PCR worker
class pcrWorker : public timeStampWorker
{
public:
    pcrWorker(std::ifstream* m_tsFile, unsigned int pid, Chart * chart);
    virtual void run();
};

////////////////////
// PTS worker
class ptsWorker : public timeStampWorker
{
public:
    ptsWorker(std::ifstream* m_tsFile, unsigned int pid, Chart * chart);
    virtual void run();
};

////////////////////
// DTS worker
class dtsWorker : public timeStampWorker
{
public:
    dtsWorker(std::ifstream* m_tsFile, unsigned int pid, Chart * chart);
    virtual void run();
};

#endif // THREAD_H
