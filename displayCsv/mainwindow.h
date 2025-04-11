#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <set>
#include <QPainter>
#include <QFileDialog>
#include <QFileInfo>
#include <QTableWidgetItem>
#include "appcontext.h"
#include "dataStorage.h"
#include "status.h"
#include "graphconsts.h"
#include "param.h"
#include "iterator.h"
#include "logic.h"
#include "ui_mainwindow.h"
#include "entrypoint.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    AppContext* context;
    AppParam* param;
    LineInfo* fileinfo;
    GraphBounds* graphBounds;
    void updateColumnsComboBox();
    void updateRegionsComboBox();
    void displayTable(Status status);
    void displaySummary();
    void updateTableRow(int row, const DemographicData* data);
    void loadFileButton();
    void loadDataButton();
    void calculateButton();
    void displayMetrics();
    void errorFileOpen();
    void errorNoData();
    void errorLoadData();
    void drawGraph();
    void extractGraphData(QVector<double>& years, QVector<double>& values);
    double getValueForCurrentColumn(const DemographicData* data);
    void calculateGraphBounds(const QVector<double>& years, const QVector<double>& values);
    void setupGraphArea(QPainter& painter, const QSize& size);
    QVector<QPointF> calculatePointPositions(const QVector<double>& years,
        const QVector<double>& values, const QSize& pixmapSize);
    void drawDataPoints(QPainter& painter, const QVector<QPointF>& points);
    void drawDataLines(QPainter& painter, const QVector<QPointF>& points);
    void markExtremesAndMedian(QPainter& painter, const QVector<QPointF>& points, const QVector<double>& values);
    int findClosestValueIndex(const QVector<double>& values, double target);
    void drawAxes(QPainter& painter, const QSize& size);
    void addAxisLabels(QPainter& painter,const QVector<QPointF>& points,
        const QVector<double>& years, const QVector<double>& values, const QSize& pixmapSize);
    bool shouldDrawXLabel(const QVector<QPointF>& points, int index);
    void disposeGraph();
};

#endif // MAINWINDOW_H
