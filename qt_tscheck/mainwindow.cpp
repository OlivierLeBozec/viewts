#include <QtWidgets>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "mainwindow.h"

MainWindow::MainWindow() :
    m_tsFile(NULL),
    m_pcrSeries(NULL),
    m_ptsSeries(NULL),
    m_dtsSeries(NULL),
    m_pcrDeltaSeries(NULL),
    m_ptsDeltaSeries(NULL),
    m_dtsDeltaSeries(NULL),
    m_pcrPtsDiffSeries(NULL),
    m_ptsDtsDiffSeries(NULL),
    m_pcrDtsDiffSeries(NULL)
{
    QWidget *main_widget = new QWidget;
    setCentralWidget(main_widget);

    createMenu();
    createLayout(main_widget);
}

MainWindow::~MainWindow()
{
    if (m_tsFile) delete m_tsFile;
}

void MainWindow::createLayout(QWidget *widget)
{
    // pcr
    m_pcrGroupBox = new QGroupBox(tr("Pcr"));
    QLabel* pcrPidLabel = new QLabel(tr("Pid:"));
    m_pcrComboBox = new QComboBox;
    m_pcrBox = new QCheckBox(tr("Display Pcr"));
    m_deltaPcrBox = new QCheckBox(tr("Display Pcr Delta"));
    m_jitterPcrBox = new QCheckBox(tr("Display Pcr Jitter"));

    QGridLayout *pcrGroupBoxLayout = new QGridLayout;
    pcrGroupBoxLayout->addWidget(pcrPidLabel, 0, 0);
    pcrGroupBoxLayout->addWidget(m_pcrComboBox, 0, 1);
    pcrGroupBoxLayout->addWidget(m_pcrBox, 1, 0);
    pcrGroupBoxLayout->addWidget(m_deltaPcrBox, 2, 0);
    pcrGroupBoxLayout->addWidget(m_jitterPcrBox, 3, 0);
    pcrGroupBoxLayout->setVerticalSpacing(0);
    m_pcrGroupBox->setLayout(pcrGroupBoxLayout);
    m_pcrGroupBox->setEnabled(false);

    connect(m_pcrBox, SIGNAL(stateChanged(int)), this, SLOT(Pcr(int)));
    connect(m_deltaPcrBox, SIGNAL(stateChanged(int)), this, SLOT(deltaPcr(int)));
    connect(m_jitterPcrBox, SIGNAL(stateChanged(int)), this, SLOT(jitterPcr(int)));
    connect(m_pcrComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(erasePcrSeries(int)));

    // pts
    m_ptsGroupBox = new QGroupBox(tr("Pts"));
    QLabel* ptsPidLabel = new QLabel(tr("Pid:"));
    m_ptsComboBox = new QComboBox;
    m_ptsBox = new QCheckBox(tr("Display Pts"));
    m_deltaPtsBox = new QCheckBox(tr("Display Pts Delta"));

    QGridLayout *ptsGroupBoxLayout = new QGridLayout;
    ptsGroupBoxLayout->addWidget(ptsPidLabel, 0, 0);
    ptsGroupBoxLayout->addWidget(m_ptsComboBox, 0, 1);
    ptsGroupBoxLayout->addWidget(m_ptsBox, 1, 0);
    ptsGroupBoxLayout->addWidget(m_deltaPtsBox, 2, 0);
    ptsGroupBoxLayout->setVerticalSpacing(0);
    m_ptsGroupBox->setLayout(ptsGroupBoxLayout);
    m_ptsGroupBox->setEnabled(false);

    connect(m_ptsBox, SIGNAL(stateChanged(int)), this, SLOT(Pts(int)));
    connect(m_deltaPtsBox, SIGNAL(stateChanged(int)), this, SLOT(deltaPts(int)));
    connect(m_ptsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(erasePtsSeries(int)));

    // dts
    m_dtsGroupBox = new QGroupBox(tr("Dts"));
    QLabel* dtsPidLabel = new QLabel(tr("Pid:"));
    m_dtsComboBox = new QComboBox;
    m_dtsBox = new QCheckBox(tr("Display Dts"));
    m_deltaDtsBox = new QCheckBox(tr("Display Dts Delta"));

    QGridLayout *dtsGroupBoxLayout = new QGridLayout;
    dtsGroupBoxLayout->addWidget(dtsPidLabel, 0, 0, Qt::AlignTop);
    dtsGroupBoxLayout->addWidget(m_dtsComboBox, 0, 1, Qt::AlignTop);
    dtsGroupBoxLayout->addWidget(m_dtsBox, 1, 0, Qt::AlignTop);
    dtsGroupBoxLayout->addWidget(m_deltaDtsBox, 2, 0, Qt::AlignTop);
    dtsGroupBoxLayout->setVerticalSpacing(0);
    m_dtsGroupBox->setLayout(dtsGroupBoxLayout);
    m_dtsGroupBox->setEnabled(false);

    connect(m_dtsBox, SIGNAL(stateChanged(int)), this, SLOT(Dts(int)));
    connect(m_deltaDtsBox, SIGNAL(stateChanged(int)), this, SLOT(deltaDts(int)));
    connect(m_dtsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(eraseDtsSeries(int)));

    // diff
    m_diffGroupBox = new QGroupBox(tr("Diff"));
    m_diffPcrPtsBox = new QCheckBox(tr("Display Pts-Pcr"));
    m_diffPtsDtsBox = new QCheckBox(tr("Display Dts-Pts"));
    m_diffPcrDtsBox = new QCheckBox(tr("Display Dts-Pcr"));

    QGridLayout *diffGroupBoxLayout = new QGridLayout;
    diffGroupBoxLayout->addWidget(m_diffPcrPtsBox, 0, 0, Qt::AlignTop);
    diffGroupBoxLayout->addWidget(m_diffPtsDtsBox, 1, 0, Qt::AlignTop);
    diffGroupBoxLayout->addWidget(m_diffPcrDtsBox, 2, 0, Qt::AlignTop);
    diffGroupBoxLayout->setVerticalSpacing(0);
    m_diffGroupBox->setLayout(diffGroupBoxLayout);
    m_diffGroupBox->setEnabled(false);

    connect(m_diffPcrPtsBox, SIGNAL(stateChanged(int)), this, SLOT(diffPcrPts(int)));
    connect(m_diffPcrDtsBox, SIGNAL(stateChanged(int)), this, SLOT(diffPcrDts(int)));
    connect(m_diffPtsDtsBox, SIGNAL(stateChanged(int)), this, SLOT(diffPtsDts(int)));

    // graph
    Chart *chart = new Chart;
    chart->setTitle("");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->createDefaultAxes();
    //chart->setAxisX();

    m_chartView = new ChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // vertical layout on left hand side
    QBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setSizeConstraint(QLayout::SetNoConstraint);
    controlLayout->addWidget(m_pcrGroupBox);
    controlLayout->addWidget(m_ptsGroupBox);
    controlLayout->addWidget(m_dtsGroupBox);
    controlLayout->addWidget(m_diffGroupBox);
    controlLayout->addStretch(); // to add space when resize

    // horizontal layout
    QBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addLayout(controlLayout);
    hlayout->addWidget(m_chartView);

    // apply
    widget->setLayout(hlayout);
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAct);

    QMenu *graphMenu = menuBar()->addMenu(tr("&Graph"));
    QAction *clearAllAct = new QAction(tr("&Clear All"), this);
    clearAllAct->setShortcuts(QKeySequence::Delete);
    clearAllAct->setStatusTip(tr("Clear all data"));
    connect(clearAllAct, &QAction::triggered, this, &MainWindow::clearAllSeries);
    graphMenu->addAction(clearAllAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&About"));
    QAction *aboutAct = helpMenu->addAction(tr("&About..."), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application About box"));
}

void MainWindow::drawSeries(QLineSeries* pSeries)
{
    if (pSeries != NULL)
    {
        Chart *chart = (Chart*)m_chartView->chart();
        chart->addSeries(pSeries);
    }
}

void MainWindow::eraseSeries(QLineSeries* pSeries)
{
    if (pSeries != NULL)
    {
        Chart *chart = (Chart*)m_chartView->chart();
        chart->removeSeries(pSeries);

        delete pSeries;
        pSeries = NULL;
    }
}

void MainWindow::erasePcrSeries(int)
{
    m_pcrBox->setChecked(false);
    m_deltaPcrBox->setChecked(false);
    m_jitterPcrBox->setChecked(false);
}

void MainWindow::erasePtsSeries(int)
{
    m_ptsBox->setChecked(false);
    m_deltaPtsBox->setChecked(false);
}

void MainWindow::eraseDtsSeries(int)
{
    m_dtsBox->setChecked(false);
    m_deltaDtsBox->setChecked(false);
}

void MainWindow::clearAllSeries()
{
    m_pcrBox->setChecked(false);
    m_deltaPcrBox->setChecked(false);
    m_jitterPcrBox->setChecked(false);
    m_ptsBox->setChecked(false);
    m_deltaPtsBox->setChecked(false);
    m_dtsBox->setChecked(false);
    m_deltaDtsBox->setChecked(false);
    m_diffPcrPtsBox->setChecked(false);
    m_diffPtsDtsBox->setChecked(false);
    m_diffPcrDtsBox->setChecked(false);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Ts"),QDir::homePath());

    if (!fileName.isEmpty())
    {
        if (m_tsFile) delete m_tsFile;
        m_tsFile = new std::ifstream(fileName.toStdString().c_str(), std::ios::binary);

        pidmap pm(*m_tsFile);

        // clear previous value
        m_pcrComboBox->clear();
        m_ptsComboBox->clear();
        m_dtsComboBox->clear();
        m_pcrGroupBox->setEnabled(false);
        m_ptsGroupBox->setEnabled(false);
        m_dtsGroupBox->setEnabled(false);
        m_diffGroupBox->setEnabled(false);
        clearAllSeries();

        // update pcr/pts/dts pid
        std::vector<unsigned int>::iterator it;
        std::vector<unsigned int> pidVect;

        bool hasPcr = false;
        pm.getPcrPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            hasPcr = true;
            m_pcrComboBox->addItem(QString::number(*it), *it);
            m_pcrGroupBox->setEnabled(true);
        }
        pidVect.clear();

        bool hasPts = false;
        pm.getPtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            hasPts = true;
            m_ptsComboBox->addItem(QString::number(*it), *it);
            m_ptsGroupBox->setEnabled(true);
        }
        pidVect.clear();

        bool hasDts = false;
        pm.getDtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            hasDts = true;
            m_dtsComboBox->addItem(QString::number(*it), *it);
            m_dtsGroupBox->setEnabled(true);
        }
        pidVect.clear();

        if (hasPcr && hasPts && hasDts)
        {
            m_diffGroupBox->setEnabled(true);
        }
        else if (hasPcr && hasPts)
        {
            // no Dts
            m_diffGroupBox->setEnabled(true);
            m_diffPtsDtsBox->setEnabled(false);
            m_diffPcrDtsBox->setEnabled(false);
        }
        else if (hasDts && hasPts)
        {
            // no Pcr
            m_diffGroupBox->setEnabled(true);
            m_diffPcrPtsBox->setEnabled(false);
            m_diffPcrDtsBox->setEnabled(false);
        }
    }
}

void MainWindow::Pcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrSeries = new QLineSeries();
        m_pcrSeries->setName(QString("Pcr"));

        // update series
        unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, pid);

        unsigned int index;
        double pcr;
        while(ts.getNextPcr(index, pcr) == true)
        {
            //std::cout << "index" << index << " - " << pcr << std::endl;
            QPointF p( index, (qreal)pcr);
            *m_pcrSeries << p;
        }

        drawSeries(m_pcrSeries);
    }
    else
    {
        eraseSeries(m_pcrSeries);
    }
}

void MainWindow::Pts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_ptsSeries = new QLineSeries();
        m_ptsSeries->setName(QString("Pts"));

        // update series
        unsigned int pid = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double pts;
        while(ts.getNextPts(index, pts) == true)
        {
            QPointF p( index, (qreal)pts);
            *m_ptsSeries << p;
        }

        drawSeries(m_ptsSeries);
    }
    else
    {
        eraseSeries(m_ptsSeries);
    }
}

void MainWindow::Dts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_dtsSeries = new QLineSeries();
        m_dtsSeries->setName(QString("Dts"));

        // update series
        unsigned int pid = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double dts;
        while(ts.getNextDts(index, dts) == true)
        {
            QPointF p( index, (qreal)dts);
            *m_dtsSeries << p;
        }

        drawSeries(m_dtsSeries);
    }
    else
    {
        eraseSeries(m_dtsSeries);
    }
}

void MainWindow::deltaPcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrDeltaSeries = new QLineSeries();
        m_pcrDeltaSeries->setName(QString("Delta Pcr"));

        // update series
        unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, pid);

        unsigned int index;
        double delta;
        while(ts.getNextDelta(index, delta) == true)
        {
            QPointF p( index, (qreal)delta);
            *m_pcrDeltaSeries << p;
        }

        drawSeries(m_pcrDeltaSeries);
    }
    else
    {
        eraseSeries(m_pcrDeltaSeries);
    }
}

void MainWindow::deltaPts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_ptsDeltaSeries = new QLineSeries();
        m_ptsDeltaSeries->setName(QString("Delta Pts"));

        // update series
        unsigned int pid = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double delta;
        while(ts.getNextDelta(index, delta) == true)
        {
            QPointF p( index, (qreal)delta);
            *m_ptsDeltaSeries << p;
        }

        drawSeries(m_ptsDeltaSeries);
    }
    else
    {
        eraseSeries(m_ptsDeltaSeries);
    }
}

void MainWindow::deltaDts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_dtsDeltaSeries = new QLineSeries();
        m_dtsDeltaSeries->setName(QString("Delta Dts"));

        // update series
        unsigned int pid = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double delta;
        while(ts.getNextDelta(index, delta) == true)
        {
            QPointF p( index, (qreal)delta);
            *m_dtsDeltaSeries << p;
        }

        drawSeries(m_dtsDeltaSeries);
    }
    else
    {
        eraseSeries(m_dtsDeltaSeries);
    }
}

void MainWindow::jitterPcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrJitterSeries = new QLineSeries();
        m_pcrJitterSeries->setName(QString("Pcr jitter"));

        // update series
        unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, pid);

        unsigned int index;
        double jitter;
        while(ts.getNextJitterPcr(index, jitter) == true)
        {
            QPointF p( index, (qreal)jitter);
            *m_pcrJitterSeries << p;
        }

        drawSeries(m_pcrJitterSeries);
    }
    else
    {
        eraseSeries(m_pcrJitterSeries);
    }
}

void MainWindow::diffPcrPts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrPtsDiffSeries = new QLineSeries();
        m_pcrPtsDiffSeries->setName(QString("(Pts-Pcr)"));

        // update series
        unsigned int pidPcr = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        unsigned int pidPts = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, pidPcr, pidPts);

        unsigned int index;
        double diff;
        while(ts.getNextDiff(index, diff) == true)
        {
            QPointF p( index, (qreal)diff);
            *m_pcrPtsDiffSeries << p;
        }

        drawSeries(m_pcrPtsDiffSeries);
    }
    else
    {
        eraseSeries(m_pcrPtsDiffSeries);
    }
}

void MainWindow::diffPtsDts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_ptsDtsDiffSeries = new QLineSeries();
        m_ptsDtsDiffSeries->setName(QString("(Pts-Pcr)"));

        // update series
        unsigned int pidPts = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        unsigned int pidDts = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pidPts, pidDts);

        unsigned int index;
        double diff;
        while(ts.getNextDiff(index, diff) == true)
        {
            QPointF p( index, (qreal)diff);
            *m_ptsDtsDiffSeries << p;
        }

        drawSeries(m_ptsDtsDiffSeries);
    }
    else
    {
        eraseSeries(m_ptsDtsDiffSeries);
    }
}

void MainWindow::diffPcrDts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrDtsDiffSeries = new QLineSeries();
        m_pcrDtsDiffSeries->setName(QString("(Dts-Pcr)"));

        // update series
        unsigned int pidPcr = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        unsigned int pidDts = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, pidPcr, TIMESTAMP_NO_PID, pidDts);

        unsigned int index;
        double diff;
        while(ts.getNextDiff(index, diff) == true)
        {
            QPointF p( index, (qreal)diff);
            *m_pcrDtsDiffSeries << p;
        }

        drawSeries(m_pcrDtsDiffSeries);
    }
    else
    {
        eraseSeries(m_pcrDtsDiffSeries);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> display pcr, pts, dts "
               "and various opreation on these timestamps "));
}
