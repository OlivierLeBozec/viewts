#ifndef WORKERSFLAGS_H
#define WORKERSFLAGS_H

#include <QDebug>
#include <QThread>
#include <QRunnable>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>

#include "chart.h"
#include "workers.h"
#include "../src/timestamp.h"

class workerFlag : public timeStampWorker
{
protected:
    // series of data to draw
    QScatterSeries* m_scatterSeries;
    QLineSeries* m_YSeries;
public:
    workerFlag(std::string &tsFile, Chart *chart, QLineSeries *series);
    ~workerFlag();

    // manipulate the series
    void showSeries();
    void hideSeries();
};


////////////////////
// flag worker
class ccWorker : public workerFlag
{
    unsigned int m_pid;

public:
    ccWorker(std::string &tsFile, unsigned int pid, Chart *chart, QLineSeries *Yseries);
    bool getData(unsigned int& index, double& val) {
        bool ret;
        unsigned int pid = 0;
        do{
            ret = m_timestamp->getNextCC(index, pid);
        } while (ret == true && pid != m_pid);

        // y is the value of the provided serie
        val = m_YSeries->at(index).y();
        return ret;
    }
};

class rapFlagWorker : public workerFlag
{
    unsigned int m_pid;
    unsigned int m_index;

public:
    rapFlagWorker(std::string &tsFile, unsigned int pid, Chart *chart, QLineSeries *Yseries);
    bool getData(unsigned int& index, double& val) {
        bool ret;
        unsigned int pid;
        do{
            ret = m_timestamp->getNextRap(index, pid);
        } while (ret == true && pid != m_pid);

        // y is the value of the provided serie
        val = m_YSeries->at(index).y();

        return ret;
    }
};

#endif // WORKERSFLAGS_H
