#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QMainWindow>

#include "chart.h"
#include "chartview.h"

#include "../src/timestamp.h"
#include "../src/pidmap.h"

QT_BEGIN_NAMESPACE
//class QAction;
//class QComboBox;
//class QMenu;
//class QLabel;
//class QChart;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

private slots:
    void Pcr(int state);
    void deltaPcr(int state);
    void jitterPcr(int state);
    void Pts(int state);
    void deltaPts(int state);
    void Dts(int state);
    void deltaDts(int state);

    void openFile();
    void clearAllChart();
    void about();

private:
    std::ifstream* m_tsFile;

    ChartView *m_chartView;

    QComboBox *m_pcrComboBox;
    QComboBox *m_ptsComboBox;
    QComboBox *m_dtsComboBox;

    QGroupBox* m_pcrGroupBox;
    QGroupBox* m_ptsGroupBox;
    QGroupBox* m_dtsGroupBox;

    QCheckBox* m_pcrBox;
    QCheckBox* m_deltaPcrBox;
    QCheckBox* m_jitterPcrBox;
    QCheckBox* m_ptsBox;
    QCheckBox* m_deltaPtsBox;
    QCheckBox* m_dtsBox;
    QCheckBox* m_deltaDtsBox;

    QLineSeries *m_pcrSeries;
    QLineSeries *m_ptsSeries;
    QLineSeries *m_dtsSeries;
    QLineSeries *m_pcrDeltaSeries;
    QLineSeries *m_ptsDeltaSeries;
    QLineSeries *m_dtsDeltaSeries;
    QLineSeries *m_pcrJitterSeries;

    void createMenu();
    void createLayout(QWidget *widget);

    void drawSeries(QLineSeries* Series, const QString legend);
    void eraseSeries(QLineSeries* Series);
};

#endif // MAINWINDOW_H
