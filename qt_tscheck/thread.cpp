#include "thread.h"

timeStampWorker::timeStampWorker(Chart *chart) :
    m_chart(chart)

{
    // new drawing
    m_Series = new QLineSeries();
    m_chart->addSeries(m_Series);

    m_axisX = new QValueAxis;
    m_axisX->setLabelFormat("%i");
    m_axisX->setTitleText("Packet index");
    m_chart->setAxisX(m_axisX, m_Series);

    m_axisY = new QValueAxis;
    m_axisY->setLabelFormat("%i");
    m_chart->setAxisY(m_axisY, m_Series);

    // do not erase series when worker is completed
    setAutoDelete (false);
}

timeStampWorker::~timeStampWorker()
{
    delete m_timestamp;
    m_chart->removeSeries(m_Series);
    delete m_Series;
    delete m_axisY;
    delete m_axisX;
}

void timeStampWorker::updateChart()
{
    qDebug() << "updateChart " << m_Series->count();

    if (m_Series->count() > 2) {
        m_chart->axisX()->setRange(m_Series->at(0).x(), m_Series->at(m_Series->count()-1).x());
        m_chart->axisY()->setRange(m_Series->at(0).y(), m_Series->at(m_Series->count()-1).y());

        emit updated();
    }
}

////////////////////
// PCR worker
pcrWorker::pcrWorker(std::ifstream* m_tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(chart)

{
    // customize base class
    m_timestamp = new timestamp(*m_tsFile, pid);
    m_axisY->setTitleText("Pcr in seconds");
    m_Series->setName(QString("Pcr"));
}

void pcrWorker::run()
{
    while (m_timestamp->run(1000) == true)
    {
        unsigned int index;
        double pcr;
        while(m_timestamp->getNextPcr(index, pcr) == true)
        {
            qDebug() << "index " << index << " - " << pcr;
            m_Series->append( index, (qreal)pcr);
        }
        updateChart();
    }

    qDebug() << "finished " << m_Series->count();
}

////////////////////
// PTS worker
ptsWorker::ptsWorker(std::ifstream* m_tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(chart)

{
    // customize base class
    m_timestamp = new timestamp(*m_tsFile, TIMESTAMP_NO_PID, pid);
    m_axisY->setTitleText("Pts in seconds");
    m_Series->setName(QString("Pts"));
}

void ptsWorker::run()
{
    while (m_timestamp->run(1000) == true)
    {
        unsigned int index;
        double pts;
        while(m_timestamp->getNextPts(index, pts) == true)
        {
            qDebug() << "index " << index << " - " << pts;
            m_Series->append( index, (qreal)pts);
        }
        updateChart();
    }

    qDebug() << "finished " << m_Series->count();
}

////////////////////
// DTS worker
dtsWorker::dtsWorker(std::ifstream* m_tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(chart)

{
    // customize base class
    m_timestamp = new timestamp(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);
    m_axisY->setTitleText("Dts in seconds");
    m_Series->setName(QString("Dts"));
}

void dtsWorker::run()
{
    while (m_timestamp->run(1000) == true)
    {
        unsigned int index;
        double dts;
        while(m_timestamp->getNextDts(index, dts) == true)
        {
            qDebug() << "index " << index << " - " << dts;
            m_Series->append( index, (qreal)dts);
        }
        updateChart();
    }

    qDebug() << "finished " << m_Series->count();
}
