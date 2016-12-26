#include "thread.h"

timeStampWorker::timeStampWorker(std::ifstream *tsFile, Chart *chart) :
    m_chart(chart), m_nbProgress(0), m_progress(0)

{
    // new drawing
    m_Series = new QLineSeries();

    // don't do that otherwise QChart is slow when updating the series
    //m_chart->addSeries(m_Series);

    // do not erase series when worker is completed
    setAutoDelete (false);

    // get file size
    tsFile->clear();
    tsFile->seekg (0, std::ios::end);
    m_fileSize = (unsigned int)tsFile->tellg();
    tsFile->seekg (0, std::ios::beg);
}

timeStampWorker::~timeStampWorker()
{
    delete m_timestamp;
    m_chart->removeSeries(m_Series);
    delete m_Series;
}

void timeStampWorker::updateProgress()
{    
    m_nbProgress += m_WindowPacket;
    if (m_fileSize)
    {
        float tmp = m_nbProgress*188;
        tmp /= m_fileSize;
        tmp *= 100;
        emit updated((int)tmp);
    }
}

void timeStampWorker::updateChart()
{
    m_chart->addSeries(m_Series);
    m_chart->createDefaultAxes();
}

void timeStampWorker::hideChart()
{
    m_chart->removeSeries(m_Series);
}

////////////////////
// PCR worker
pcrWorker::pcrWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Pcr in seconds"));
}

void pcrWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double pcr;
        while(m_timestamp->getNextPcr(index, pcr) == true)
        {
            //qDebug() << "index " << index << " - " << pcr;
            m_Series->append( index, (qreal)pcr);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// Delta PCR worker
pcrDeltaWorker::pcrDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Delta Pcr in seconds"));
}

void pcrDeltaWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double delta;
        while(m_timestamp->getNextDelta(index, delta) == true)
        {
            //qDebug() << "index " << index << " - " << delta;
            m_Series->append( index, (qreal)delta);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// Jitter PCR worker
pcrJitterWorker::pcrJitterWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Jitter Pcr in sceonds"));
}

void pcrJitterWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double jitter;
        while(m_timestamp->getNextJitterPcr(index, jitter) == true)
        {
            //qDebug() << "index " << index << " - " << jitter;
            m_Series->append( index, (qreal)jitter);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

////////////////////
// PTS worker
ptsWorker::ptsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Pts in seconds"));
}

void ptsWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double pts;
        while(m_timestamp->getNextPts(index, pts) == true)
        {
            //qDebug() << "index " << index << " - " << pts;
            m_Series->append( index, (qreal)pts);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// PTS Delta worker
ptsDeltaWorker::ptsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Pts delta in seconds"));
}

void ptsDeltaWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double delta;
        while(m_timestamp->getNextDelta(index, delta) == true)
        {
            //qDebug() << "index " << index << " - " << delta;
            m_Series->append( index, (qreal)delta);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

////////////////////
// DTS worker
dtsWorker::dtsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Dts in seconds"));
}

void dtsWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double dts;
        while(m_timestamp->getNextDts(index, dts) == true)
        {
            //qDebug() << "index " << index << " - " << dts;
            m_Series->append( index, (qreal)dts);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// DTS worker
dtsDeltaWorker::dtsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Dts delta in seconds"));
}

void dtsDeltaWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double delta;
        while(m_timestamp->getNextDelta(index, delta) == true)
        {
            //qDebug() << "index " << index << " - " << delta;
            m_Series->append( index, (qreal)delta);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

////////////////
// Diff PCR PTS worker
diffPcrPtsWorker::diffPcrPtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pidPcr, pidPts);
    m_Series->setName(QString("Pts-Pcr in seconds"));
}

void diffPcrPtsWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double diff;
        while(m_timestamp->getNextDiff(index, diff) == true)
        {
            //qDebug() << "index " << index << " - " << diff;
            m_Series->append( index, (qreal)diff);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// Diff PCR DTS worker
diffPcrDtsWorker::diffPcrDtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pidPcr, TIMESTAMP_NO_PID, pidDts);
    m_Series->setName(QString("Dts-Pcr in seconds"));
}

void diffPcrDtsWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double diff;
        while(m_timestamp->getNextDiff(index, diff) == true)
        {
            //qDebug() << "index " << index << " - " << diff;
            m_Series->append( index, (qreal)diff);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}

// Diff PTS DTS worker
diffPtsDtsWorker::diffPtsDtsWorker(std::ifstream *tsFile, unsigned int pidPts, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pidPts, pidDts);
    m_Series->setName(QString("Dts-Pts in seconds"));
}

void diffPtsDtsWorker::run()
{
    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double diff;
        while(m_timestamp->getNextDiff(index, diff) == true)
        {
            //qDebug() << "index " << index << " - " << diff;
            m_Series->append( index, (qreal)diff);
        }
        updateProgress();
    }

    emit updated(100);
    emit finished();
}
