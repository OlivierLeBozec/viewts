#include "workersFlags.h"

////////////////////////////////
// Timestamp worker - base class
workerFlag::workerFlag(std::string &tsFileName, Chart *chart, QLineSeries *YSeries) :
    timeStampWorker(tsFileName, chart), m_YSeries(YSeries)

{
    // new drawing
    m_scatterSeries = new QScatterSeries();
}

workerFlag::~workerFlag()
{
    m_chart->removeSeries(m_scatterSeries);
    delete(m_scatterSeries);
}

void workerFlag::showSeries()
{
    m_chart->addSeries(m_scatterSeries);
    m_chart->createDefaultAxes();
}

void workerFlag::hideSeries()
{
    m_chart->removeSeries(m_scatterSeries);
}

////////////////////
// Flag worker
ccWorker::ccWorker(std::string &tsFile, unsigned int pid, Chart *chart, QLineSeries* YSeries) :
    workerFlag(tsFile, chart, YSeries), m_pid(pid)
{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_scatterSeries->setName(QString(tr("CC")));
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scatterSeries->setMarkerSize(15.0);
}

rapFlagWorker::rapFlagWorker(std::string &tsFile, unsigned int pid, Chart *chart, QLineSeries* YSeries) :
    workerFlag(tsFile, chart, YSeries), m_pid(pid)
{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_scatterSeries->setName(QString(tr("RAP")));
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    m_scatterSeries->setMarkerSize(15.0);
}
