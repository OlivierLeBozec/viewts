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
    void openFile();
    void Pcr();
    void Pts();
    void Dts();
    void deltaPcr();
    void deltaPts();
    void deltaDts();
    void about();

private:
    std::ifstream* m_tsFile;

    ChartView *m_chartView;
    QComboBox *m_pcrComboBox;
    QComboBox *m_ptsComboBox;
    QComboBox *m_dtsComboBox;

    void createMenu();
    void UpdateChart(QLineSeries* Series);

};

#endif // MAINWINDOW_H
