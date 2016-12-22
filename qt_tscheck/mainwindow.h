#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtCharts/QLineSeries>
//#include <QtCharts/QValueAxis>
#include <QMainWindow>

#include "chart.h"
#include "chartview.h"
#include "thread.h"

#include "../src/timestamp.h"
#include "../src/pidmap.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

private slots:
    void Pcr(int state);
    //void updatePcr();
    void deltaPcr(int state);
    void jitterPcr(int state);
    void Pts(int state);
    void deltaPts(int state);
    void Dts(int state);
    void deltaDts(int state);
    void diffPcrPts(int state);
    void diffPtsDts(int state);
    void diffPcrDts(int state);

    void openFile();
    void clearAllSeries();
    void erasePcrSeries(int);
    void erasePtsSeries(int);
    void eraseDtsSeries(int);
    void about();

private:
    std::ifstream* m_tsFile;

    QThreadPool *m_pthreadPool;

    ChartView *m_chartView;

    QComboBox *m_pcrComboBox;
    QComboBox *m_ptsComboBox;
    QComboBox *m_dtsComboBox;

    QGroupBox* m_pcrGroupBox;
    QGroupBox* m_ptsGroupBox;
    QGroupBox* m_dtsGroupBox;
    QGroupBox* m_diffGroupBox;

    QCheckBox* m_pcrBox;
    QCheckBox* m_deltaPcrBox;
    QCheckBox* m_jitterPcrBox;
    QCheckBox* m_ptsBox;
    QCheckBox* m_deltaPtsBox;
    QCheckBox* m_dtsBox;
    QCheckBox* m_deltaDtsBox;
    QCheckBox* m_diffPcrPtsBox;
    QCheckBox* m_diffPtsDtsBox;
    QCheckBox* m_diffPcrDtsBox;

    QLineSeries *m_pcrDeltaSeries;
    QLineSeries *m_ptsDeltaSeries;
    QLineSeries *m_dtsDeltaSeries;
    QLineSeries *m_pcrJitterSeries;
    QLineSeries *m_pcrPtsDiffSeries;
    QLineSeries *m_ptsDtsDiffSeries;
    QLineSeries *m_pcrDtsDiffSeries;

    pcrWorker    *m_pcrWorker;
    ptsWorker    *m_ptsWorker;
    dtsWorker    *m_dtsWorker;

    void createMenu();
    void createLayout(QWidget *widget);

    void drawSeries(QLineSeries* Series);
    void eraseSeries(QLineSeries* Series);
};

#endif // MAINWINDOW_H
