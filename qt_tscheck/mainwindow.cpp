#include <QtWidgets>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "mainwindow.h"

MainWindow::MainWindow() :
    m_tsFile(NULL)
{
    QWidget *main_widget = new QWidget;
    setCentralWidget(main_widget);

    createMenu();

    // create layout
    QLabel *pcrLabel = new QLabel(tr("Pcr:"));
    pcrLabel->setFixedHeight(20);
    m_pcrComboBox = new QComboBox;
    QLabel *ptsLabel = new QLabel(tr("Pts:"));
    ptsLabel->setFixedHeight(20);
    m_ptsComboBox = new QComboBox;
    QLabel *dtsLabel = new QLabel(tr("Dts:"));
    dtsLabel->setFixedHeight(20);
    m_dtsComboBox = new QComboBox;

    QAbstractButton *PcrButton = new QPushButton(tr("Pcr"));
    connect(PcrButton, SIGNAL(clicked()), this, SLOT(Pcr()));
    QAbstractButton *deltaPcrButton = new QPushButton(tr("Delta Pcr"));
    connect(deltaPcrButton, SIGNAL(clicked()), this, SLOT(deltaPcr()));
    QAbstractButton *PtsButton = new QPushButton(tr("Pts"));
    connect(PtsButton, SIGNAL(clicked()), this, SLOT(Pts()));
    QAbstractButton *deltaPtsButton = new QPushButton(tr("Delta Pts"));
    connect(deltaPtsButton, SIGNAL(clicked()), this, SLOT(deltaPts()));
    QAbstractButton *DtsButton = new QPushButton(tr("Dts"));
    connect(DtsButton, SIGNAL(clicked()), this, SLOT(Dts()));
    QAbstractButton *deltaDtsButton = new QPushButton(tr("Delta Dts"));
    connect(deltaDtsButton, SIGNAL(clicked()), this, SLOT(deltaDts()));

    Chart *chart = new Chart;
    m_chartView = new ChartView(chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    QBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->addWidget(pcrLabel);
    controlLayout->addWidget(m_pcrComboBox);
    controlLayout->addWidget(PcrButton);
    controlLayout->addWidget(deltaPcrButton);
    controlLayout->addWidget(ptsLabel);
    controlLayout->addWidget(m_ptsComboBox);
    controlLayout->addWidget(PtsButton);
    controlLayout->addWidget(deltaPtsButton);
    controlLayout->addWidget(dtsLabel);
    controlLayout->addWidget(m_dtsComboBox);
    controlLayout->addWidget(DtsButton);
    controlLayout->addWidget(deltaDtsButton);

    QBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addLayout(controlLayout);
    hlayout->addWidget(m_chartView);

    // set layout
    main_widget->setLayout(hlayout);
}

MainWindow::~MainWindow()
{
    if (m_tsFile) delete m_tsFile;
}

void MainWindow::UpdateChart(QLineSeries* Series)
{
    Chart *chart = (Chart*)m_chartView->chart();
    chart->addSeries(Series);
    chart->setTitle("");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->createDefaultAxes();

    m_chartView->setChart(chart);
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAct);


    QMenu *helpMenu = menuBar()->addMenu(tr("&About"));
    QAction *aboutAct = helpMenu->addAction(tr("&About..."), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application About box"));
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Ts"),QDir::homePath());

    if (!fileName.isEmpty())
    {
        if (m_tsFile) delete m_tsFile;
        m_tsFile = new std::ifstream(fileName.toStdString().c_str(), std::ios::binary);

        pidmap pm(*m_tsFile);

        // update pcr/pts/dts pid
        std::vector<unsigned int>::iterator it;
        std::vector<unsigned int> pidVect;

        pm.getPcrPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_pcrComboBox->addItem(QString::number(*it), *it);
        }
        pidVect.clear();

        pm.getPtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_ptsComboBox->addItem(QString::number(*it), *it);
        }
        pidVect.clear();

        pm.getDtsPid(pidVect);
        for (it = pidVect.begin(); it < pidVect.end(); it++)
        {
            m_dtsComboBox->addItem(QString::number(*it), *it);
        }
        pidVect.clear();
    }
}

void MainWindow::Pcr()
{
    // protection
    if (m_pcrComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, pid);

    unsigned int index;
    double pcr;
    while(ts.getNextPcr(index, pcr) == true)
    {
        QPointF p( index, (qreal)pcr);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::Pts()
{
    // protection
    if (m_ptsComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_ptsComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pid);

    unsigned int index;
    double pts;
    while(ts.getNextPts(index, pts) == true)
    {
        QPointF p( index, (qreal)pts);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::Dts()
{
    // protection
    if (m_dtsComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_dtsComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);

    unsigned int index;
    double dts;
    while(ts.getNextDts(index, dts) == true)
    {
        QPointF p( index, (qreal)dts);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::deltaPcr()
{
    // protection
    if (m_pcrComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_pcrComboBox->itemData(m_pcrComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, pid);

    unsigned int index;
    double delta;
    while(ts.getNextDelta(index, delta) == true)
    {
        QPointF p( index, (qreal)delta);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::deltaPts()
{
    // protection
    if (m_ptsComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_pcrComboBox->itemData(m_ptsComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, pid);

    unsigned int index;
    double delta;
    while(ts.getNextDelta(index, delta) == true)
    {
        QPointF p( index, (qreal)delta);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::deltaDts()
{
    // protection
    if (m_dtsComboBox->count() == 0) return;

    // new drawing
    QLineSeries *series = new QLineSeries();

    // update series
    unsigned int pid = m_pcrComboBox->itemData(m_dtsComboBox->currentIndex()).toInt();
    timestamp ts(*m_tsFile, TIMESTAMP_NO_PID, TIMESTAMP_NO_PID, pid);

    unsigned int index;
    double delta;
    while(ts.getNextDelta(index, delta) == true)
    {
        QPointF p( index, (qreal)delta);
        *series << p;
    }

    UpdateChart(series);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> display pcr, pts, dts "
               "and various opreation on these timestamps "));
}
