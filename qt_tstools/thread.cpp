#include "thread.h"

////////////////////
// Info worker
infoWorker::infoWorker(std::string *tsFile, unsigned int pid) :
     m_isRunning(false)
{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
}

infoWorker::~infoWorker()
{
    delete m_timestamp;
}

void infoWorker::run()
{
    m_isRunning = true;
    m_timestamp->run();
    m_isRunning = false;

    emit finished();
}

double infoWorker::getGlobalBitrate()
{
    if (m_timestamp && m_isRunning == false)
    {
        // in byte per seconds
        return m_timestamp->getGlobalBitrate();
    }
    return 0;
}

double infoWorker::getGlobalDuration()
{
    if (m_timestamp && m_isRunning == false)
    {
        // in seconds
        return m_timestamp->getDuration();
    }
    return 0;
}

////////////////////////////////
// Timestamp worker - base class
timeStampWorker::timeStampWorker(std::string *tsFileName, Chart *chart) :
   m_nbProgress(0),  m_chart(chart), m_isTimeXaxis(true), m_isRunning(false)

{
    // new drawing
    m_Series = new QLineSeries();

    // don't do that otherwise QChart is slow when updating the series
    // series must be updated at the end in parent thread
    //m_chart->addSeries(m_Series);

    // do not erase series when worker is completed
    setAutoDelete (false);

    // get file size
    std::ifstream* fileIn =  new std::ifstream(tsFileName->c_str(), std::ios::binary);
    fileIn->seekg (0, std::ios::end);
    m_fileSize = (unsigned long long)fileIn->tellg();
    fileIn->seekg (0, std::ios::beg);
    fileIn->close();
    delete fileIn;
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
        double tmp = m_nbProgress;
        tmp *= 188;
        tmp *= 100;
        tmp /= (double)m_fileSize;
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
            if (m_isTimeXaxis)
                // time for X axis
                m_Series->append( m_timestamp->getTimeFromIndex(index), (qreal)val);
            else
                // packet index for X axis
                m_Series->append( index, (qreal)val);
            //qDebug() << m_Series->name() << " - index " << index << " - " << m_timestamp->getTimeFromIndex(index) << " ms - " << val;
        }
        updateProgress();
    }

    m_isRunning = false;
    emit updated(100);
    emit finished();
}

////////////////////
// PCR worker
pcrWorker::pcrWorker(std::string *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_Series->setName(QString(tr("Pcr")));
    m_func = &(m_timestamp->getNextPcr);
}

pcrDeltaWorker::pcrDeltaWorker(std::string *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_Series->setName(QString(tr("Pcr(n+1)-Pcr(n)")));
    m_func = &(m_timestamp->getNextDelta);
}

pcrJitterWorker::pcrJitterWorker(std::string *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_Series->setName(QString(tr("Jitter Pcr")));
    m_func = &(m_timestamp->getNextJitterPcr);
}

pcrBitrateWorker::pcrBitrateWorker(std::string *tsFile, unsigned int pid, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pid);
    m_Series->setName(QString(tr("bitrate in B/s")));
    m_func = &(m_timestamp->getNextBitrate);
}

////////////////////
// PTS worker
ptsWorker::ptsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts);
    m_Series->setName(QString(tr("Pts")));
    m_func = &(m_timestamp->getNextPts);
}

ptsDeltaWorker::ptsDeltaWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts);
    m_Series->setName(QString(tr("Pts(n+1))-Pts(n)")));
    m_func = &(m_timestamp->getNextDelta);
}

////////////////////
// DTS worker
dtsWorker::dtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, TIMESTAMP_NO_PID, pidDts);
    m_Series->setName(QString(tr("Dts")));
    m_func = &(m_timestamp->getNextDts);
}

dtsDeltaWorker::dtsDeltaWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, TIMESTAMP_NO_PID, pidPts);
    m_Series->setName(QString(tr("Dts(n+1)-Dts(n)")));
    m_func = &(m_timestamp->getNextDelta);
}

////////////////
// Diff worker
diffPcrPtsWorker::diffPcrPtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts);
    m_Series->setName(QString(tr("Pts(n)-Pcr(n)")));
    m_func = &(m_timestamp->getNextDiff);
}

diffPcrDtsWorker::diffPcrDtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, TIMESTAMP_NO_PID, pidDts);
    m_Series->setName(QString(tr("Dts(n)-Pcr(n)")));
    m_func = &(m_timestamp->getNextDiff);
}

diffPtsDtsWorker::diffPtsDtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts, pidDts);
    m_Series->setName(QString(tr("Pts(n)-Dts(n)")));
    m_func = &(m_timestamp->getNextDiff);
}

buffLevelPtsWorker::buffLevelPtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts);
    m_Series->setName(QString(tr("ES buffer level using PTS")));
    m_func = &(m_timestamp->getNextLevel);
}

buffLevelPtsDtsWorker::buffLevelPtsDtsWorker(std::string *tsFile, unsigned int pidPcr, unsigned int pidPts, unsigned int pidDts, Chart *chart) :
    timeStampWorker(tsFile, chart)

{
    // customize base class
    m_timestamp = new timestamp(tsFile, pidPcr, pidPts, pidDts);
    m_Series->setName(QString(tr("PES buffer level using PTS/DTS")));
    m_func = &(m_timestamp->getNextLevel);
}
