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

public:
    workerFlag(std::string &tsFile, Chart *chart);
    ~workerFlag();

    // write the serie of points in file
    virtual void serializeSeries(std::ofstream *outFile);

    // run function from QRunnable
    void run();

    // manipulate the series
    virtual void showSeries();
    virtual void hideSeries();
};


////////////////////
// flag worker
class ccWorker : public workerFlag
{
    unsigned int m_pid;

public:
    ccWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        bool ret;
        unsigned int pid = 0;
        do{
            ret = m_timestamp->getNextCC(index, pid);
        } while (ret == true && pid != m_pid);

        // y is the value of the index
        val = index;
        return ret;
    }
};

class rapFlagWorker : public workerFlag
{
    unsigned int m_pid;
    unsigned int m_index;

public:
    rapFlagWorker(std::string &tsFile, unsigned int pid, Chart *chart);
    bool getData(unsigned int& index, double& val) {
        bool ret;
        unsigned int pid;
        do{
            ret = m_timestamp->getNextRap(index, pid);
        } while (ret == true && pid != m_pid);

        // y is the value of the index
        val = index;

        return ret;
    }
};

#endif // WORKERSFLAGS_H
