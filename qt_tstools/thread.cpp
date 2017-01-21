#include "thread.h"

timeStampWorker::timeStampWorker(std::ifstream *tsFile, Chart *chart) :
   m_nbProgress(0), m_progress(0),  m_chart(chart), m_isRunning(false)

{
    // new drawing
    m_Series = new QLineSeries();

    // don't do that otherwise QChart is slow when updating the series
    // series must be updated at the end
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
    // update percent
    m_nbProgress += m_WindowPacket;
    if (m_fileSize)
    {
        float tmp = m_nbProgress*188;
        tmp /= m_fileSize;
        tmp *= 100;
        emit updated((int)tmp);
    }
}

void timeStampWorker::showSeries()
{
    m_chart->addSeries(m_Series);
    m_chart->createDefaultAxes();
}

void timeStampWorker::hideSeries()
{
    m_chart->removeSeries(m_Series);
}

void timeStampWorker::serializeSeries(std::ofstream *outFile)
{
    *outFile << m_Series->name().toStdString().c_str() << std::endl;
    for (int i = 0; i < m_Series->count(); i++)
    {
        QString line = QString::number(m_Series->at(i).x(), 'f', 0) + ", " + QString::number(m_Series->at(i).y(), 'f', 9);
        *outFile << line.toStdString().c_str() << std::endl;
    }
}

void timeStampWorker::run()
{
    m_isRunning = true;

    while (m_timestamp->run(m_WindowPacket) == true)
    {
        unsigned int index;
        double val;
        while((m_timestamp->*m_func)(index, val) == true)
        {
            //qDebug() << m_Series->name() << " - index " << index << " - " << val;
            m_Series->append( index, (qreal)val);
        }
        updateProgress();
    }

    m_isRunning = false;
    emit updated(100);
    emit finished();
}

////////////////////
// PCR worker
pcrWorker::pcrWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Pcr in seconds"));
    m_func = &(m_timestamp->getNextPcr);
}

pcrDeltaWorker::pcrDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Delta Pcr in seconds"));
    m_func = &(m_timestamp->getNextDelta);
}

pcrJitterWorker::pcrJitterWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("Jitter Pcr in seconds"));
    m_func = &(m_timestamp->getNextJitterPcr);
}

pcrBitrateWorker::pcrBitrateWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pid);
    m_Series->setName(QString("bitrate in bit per seconds"));
    m_func = &(m_timestamp->getNextBitrate);
}

////////////////////
// PTS worker
ptsWorker::ptsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Pts in seconds"));
    m_func = &(m_timestamp->getNextPts);
}

ptsDeltaWorker::ptsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Pts delta in seconds"));
    m_func = &(m_timestamp->getNextDelta);
}

////////////////////
// DTS worker
dtsWorker::dtsWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Dts in seconds"));
    m_func = &(m_timestamp->getNextDts);
}

dtsDeltaWorker::dtsDeltaWorker(std::ifstream *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);
    m_Series->setName(QString("Dts delta in seconds"));
    m_func = &(m_timestamp->getNextDelta);
}

////////////////
// Diff worker
diffPcrPtsWorker::diffPcrPtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pidPcr, pidPts);
    m_Series->setName(QString("Pts-Pcr in seconds"));
    m_func = &(m_timestamp->getNextDiff);
}

diffPcrDtsWorker::diffPcrDtsWorker(std::ifstream *tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, pidPcr, TIMESTAMP_NO_PID, pidDts);
    m_Series->setName(QString("Dts-Pcr in seconds"));
    m_func = &(m_timestamp->getNextDiff);
}

diffPtsDtsWorker::diffPtsDtsWorker(std::ifstream *tsFile, unsigned int pidPts, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(*tsFile, TIMESTAMP_NO_PID, pidPts, pidDts);
    m_Series->setName(QString("Dts-Pts in seconds"));
    m_func = &(m_timestamp->getNextDiff);
}
