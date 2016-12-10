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
    m_dtsDeltaSeries(NULL)
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

    // graph
    Chart *chart = new Chart;
    chart->setTitle("");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->createDefaultAxes();

    m_chartView = new ChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // vertical layout
    QBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setSizeConstraint(QLayout::SetNoConstraint);
    controlLayout->addWidget(m_pcrGroupBox);
    controlLayout->addWidget(m_ptsGroupBox);
    controlLayout->addWidget(m_dtsGroupBox);
    controlLayout->addStretch();

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
    connect(clearAllAct, &QAction::triggered, this, &MainWindow::clearAllChart);
    graphMenu->addAction(clearAllAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&About"));
    QAction *aboutAct = helpMenu->addAction(tr("&About..."), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application About box"));
}

void MainWindow::drawSeries(QLineSeries* Series, const QString legend)
{
    Chart *chart = (Chart*)m_chartView->chart();
    chart->addSeries(Series);
    //chart->setsetTitle(legend);
}

void MainWindow::eraseSeries(QLineSeries* Series)
{
    Chart *chart = (Chart*)m_chartView->chart();
    chart->removeSeries(Series);
}

void MainWindow::clearAllChart()
{
    Chart *chart = (Chart*)m_chartView->chart();
    chart->removeAllSeries();

    m_pcrBox->setChecked(false);
    m_deltaPcrBox->setChecked(false);
    m_jitterPcrBox->setChecked(false);
    m_ptsBox->setChecked(false);
    m_deltaPtsBox->setChecked(false);
    m_dtsBox->setChecked(false);
    m_deltaDtsBox->setChecked(false);
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
        clearAllChart();

        // update pcr/pts/dts pid
        std::vector<unsigned int>::iterator it;
        std::vector<unsigned int> pidVect;

        pm.getPcrPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_pcrComboBox->addItem(QString::number(*it), *it);
            m_pcrGroupBox->setEnabled(true);
        }
        pidVect.clear();

        pm.getPtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_ptsComboBox->addItem(QString::number(*it), *it);
            m_ptsGroupBox->setEnabled(true);
        }
        pidVect.clear();

        pm.getDtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_dtsComboBox->addItem(QString::number(*it), *it);
            m_dtsGroupBox->setEnabled(true);
        }
        pidVect.clear();
    }
}

void MainWindow::Pcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrSeries = new QLineSeries();

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

        drawSeries(m_pcrSeries, "Pcr");
    }
    else
    {
        eraseSeries(m_pcrSeries);
        delete m_pcrSeries;
    }
}

void MainWindow::Pts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_ptsSeries = new QLineSeries();

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

        drawSeries(m_ptsSeries, "Pts");
    }
    else
    {
        eraseSeries(m_ptsSeries);
        delete m_ptsSeries;
    }
}

void MainWindow::Dts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_dtsSeries = new QLineSeries();

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

        drawSeries(m_dtsSeries, "Dts");
    }
    else
    {
        eraseSeries(m_dtsSeries);
        delete m_dtsSeries;
    }
}

void MainWindow::deltaPcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrDeltaSeries = new QLineSeries();

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

        drawSeries(m_pcrDeltaSeries, "delta Pcr");
    }
    else
    {
        eraseSeries(m_pcrDeltaSeries);
        delete m_pcrDeltaSeries;
    }
}

void MainWindow::deltaPts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_ptsDeltaSeries = new QLineSeries();

        // update series
        unsigned int pid = m_pcrComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double delta;
        while(ts.getNextDelta(index, delta) == true)
        {
            QPointF p( index, (qreal)delta);
            *m_ptsDeltaSeries << p;
        }

        drawSeries(m_ptsDeltaSeries, "delta Pts");
    }
    else
    {
        eraseSeries(m_ptsDeltaSeries);
        delete m_ptsDeltaSeries;
    }
}

void MainWindow::deltaDts(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_dtsDeltaSeries = new QLineSeries();

        // update series
        unsigned int pid = m_pcrComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
        timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);

        unsigned int index;
        double delta;
        while(ts.getNextDelta(index, delta) == true)
        {
            QPointF p( index, (qreal)delta);
            *m_dtsDeltaSeries << p;
        }

        drawSeries(m_dtsDeltaSeries, "delta Dts");
    }
    else
    {
        eraseSeries(m_dtsDeltaSeries);
        delete m_dtsDeltaSeries;
    }
}

void MainWindow::jitterPcr(int state)
{
    if (state == Qt::Checked)
    {
        // new drawing
        m_pcrJitterSeries = new QLineSeries();

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

        drawSeries(m_pcrJitterSeries, "Pcr jitter");
    }
    else
    {
        eraseSeries(m_pcrJitterSeries);
        delete m_pcrJitterSeries;
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> display pcr, pts, dts "
               "and various opreation on these timestamps "));
}
