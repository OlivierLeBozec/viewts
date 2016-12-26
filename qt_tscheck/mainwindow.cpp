#include "mainwindow.h"
#include "thread.h"

MainWindow::MainWindow() :
    m_tsFile(Q_NULLPTR),
    m_pthreadPool(QThreadPool::globalInstance()),
    m_pcrWorker(Q_NULLPTR)
{
    QWidget *main_widget = new QWidget;
    setCentralWidget(main_widget);

    createMenu();
    createLayout(main_widget);
}

MainWindow::~MainWindow()
{
    if (m_tsFile) delete m_tsFile;

    m_pthreadPool->cancel(m_pcrWorker);
    delete m_pcrWorker;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> display pcr, pts, dts "
               "and various opreation on these timestamps "));
}

void MainWindow::updateStatusBar(int percent)
{
    qDebug() << "updateStatusBar " << percent;

    if (percent == 100) {
        statusBar()->showMessage(tr("Done..."), 5000);
    }
    else {
        QString message = QString(tr("Processing %1%")).arg(QString::number(percent));
        statusBar()->showMessage(message, 1000);
    }
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
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setTheme(QChart::ChartThemeDark);

    m_chartView = new ChartView(chart);
    //m_chartView->setRenderHint(QPainter::Antialiasing);

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
        pm.run(5000);

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

/////////////////////////////////////////////// Chart
void MainWindow::Pcr(int state)
{
    if (state == Qt::Checked)
    {
        if (m_pcrWorker == Q_NULLPTR) {
            // build serie
            unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
            m_pcrWorker = new pcrWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
            connect(m_pcrWorker, SIGNAL(finished()), this, SLOT(updatePcr()));
            connect(m_pcrWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
            m_pthreadPool->start(m_pcrWorker);
        }
        else {
            // redraw serie
            m_pcrWorker->updateChart();
            connect(m_pcrWorker, SIGNAL(finished()), this, SLOT(updatePcr()));
            connect(m_pcrWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        }
    }
    else
    {
        // undraw serie
        //m_pthreadPool->cancel(m_pcrWorker);
        disconnect(m_pcrWorker, SIGNAL(finished()), this, SLOT(updatePcr()));
        disconnect(m_pcrWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pcrWorker->hideChart();
    }
}

void  MainWindow::updatePcr()
{
    // should be done in parent thread
    m_pcrWorker->updateChart();
}

void MainWindow::Pts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        m_ptsWorker = new ptsWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_ptsWorker, SIGNAL(finished()), this, SLOT(updatePts()));
        connect(m_ptsWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_ptsWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_ptsWorker);
        delete m_ptsWorker;
    }
}

void  MainWindow::updatePts()
{
    // should be done in parent thread
    m_ptsWorker->updateChart();
}

void MainWindow::Dts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        m_dtsWorker = new dtsWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_dtsWorker, SIGNAL(finished()), this, SLOT(updateDts()));
        connect(m_dtsWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_dtsWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_dtsWorker);
        delete m_dtsWorker;
    }
}

void  MainWindow::updateDts()
{
    // should be done in parent thread
    m_dtsWorker->updateChart();
}

void MainWindow::deltaPcr(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        m_deltaPcrWorker = new pcrDeltaWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_deltaPcrWorker, SIGNAL(finished()), this, SLOT(updateDeltaPcr()));
        connect(m_deltaPcrWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_deltaPcrWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_deltaPcrWorker);
        delete m_deltaPcrWorker;
    }
}

void MainWindow::updateDeltaPcr()
{
    // should be done in parent thread
    m_deltaPcrWorker->updateChart();
}

void MainWindow::deltaPts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        m_ptsDeltaWorker = new ptsDeltaWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_ptsDeltaWorker, SIGNAL(finished()), this, SLOT(updatePtsDelta()));
        connect(m_ptsDeltaWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_ptsDeltaWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_ptsDeltaWorker);
        delete m_ptsDeltaWorker;
    }
}

void  MainWindow::updatePtsDelta()
{
    // should be done in parent thread
    m_ptsDeltaWorker->updateChart();
}


void MainWindow::deltaDts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        m_dtsDeltaWorker = new dtsDeltaWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_dtsDeltaWorker, SIGNAL(finished()), this, SLOT(updateDtsDelta()));
        connect(m_dtsDeltaWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_dtsDeltaWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_dtsDeltaWorker);
        delete m_dtsDeltaWorker;
    }
}

void  MainWindow::updateDtsDelta()
{
    // should be done in parent thread
    m_dtsDeltaWorker->updateChart();
}

void MainWindow::jitterPcr(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        m_jitterPcrWorker = new pcrJitterWorker(m_tsFile, pid, (Chart*)m_chartView->chart());
        connect(m_jitterPcrWorker, SIGNAL(finished()), this, SLOT(updateJitterPcr()));
        connect(m_jitterPcrWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_jitterPcrWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_jitterPcrWorker);
        delete m_jitterPcrWorker;
    }
}

void MainWindow::updateJitterPcr()
{
    // should be done in parent thread
    m_jitterPcrWorker->updateChart();
}

void MainWindow::diffPcrPts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pidPcr = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        unsigned int pidPts = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        m_diffPcrPtsWorker = new diffPcrPtsWorker(m_tsFile, pidPcr, pidPts, (Chart*)m_chartView->chart());
        connect(m_diffPcrPtsWorker, SIGNAL(finished()), this, SLOT(updateDiffPcrPts()));
        connect(m_diffPcrPtsWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_diffPcrPtsWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_diffPcrPtsWorker);
        delete m_diffPcrPtsWorker;
    }
}

void MainWindow::updateDiffPcrPts()
{
    // should be done in parent thread
    m_diffPcrPtsWorker->updateChart();
}


void MainWindow::diffPcrDts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pidPcr = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
        unsigned int pidDts = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        m_diffPcrDtsWorker = new diffPcrDtsWorker(m_tsFile, pidPcr, pidDts, (Chart*)m_chartView->chart());
        connect(m_diffPcrDtsWorker, SIGNAL(finished()), this, SLOT(updateDiffPcrDts()));
        connect(m_diffPcrDtsWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_diffPcrDtsWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_diffPcrDtsWorker);
        delete m_diffPcrDtsWorker;
    }
}

void MainWindow::updateDiffPcrDts()
{
    // should be done in parent thread
    m_diffPcrDtsWorker->updateChart();
}

void MainWindow::diffPtsDts(int state)
{
    if (state == Qt::Checked)
    {
        unsigned int pidPts = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        unsigned int pidDts = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        m_diffPtsDtsWorker = new diffPtsDtsWorker(m_tsFile, pidPts, pidDts, (Chart*)m_chartView->chart());
        connect(m_diffPtsDtsWorker, SIGNAL(finished()), this, SLOT(updateDiffPtsDts()));
        connect(m_diffPtsDtsWorker, SIGNAL(updated(int)), this, SLOT(updateStatusBar(int)));
        m_pthreadPool->start(m_diffPtsDtsWorker);
    }
    else
    {
        m_pthreadPool->cancel(m_diffPtsDtsWorker);
        delete m_diffPtsDtsWorker;
    }
}

void MainWindow::updateDiffPtsDts()
{
    // should be done in parent thread
    m_diffPtsDtsWorker->updateChart();
}
