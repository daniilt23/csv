#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , context(new AppContext{})
    , param(new AppParam)
    , graphBounds(new GraphBounds)
{
    ui->setupUi(this);
    QString comboBoxStyle =
        "QComboBox "
        "{"
        "    border: 1px solid gray;"
        "    border-radius: 3px;"
        "    padding: 1px 18px 1px 3px;"
        "}"
        "QComboBox:focus "
        "{"
        "    border-color: gray;"
        "    outline: none;"
        "}";

    ui->regionComboBox->setStyleSheet(comboBoxStyle);
    ui->columnComboBox->setStyleSheet(comboBoxStyle);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadFileButton);
    connect(ui->loadDataButton, &QPushButton::clicked, this, &MainWindow::loadDataButton);
    connect(ui->calculateButton, &QPushButton::clicked, this, &MainWindow::calculateButton);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::updateRegionsComboBox);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::updateColumnsComboBox);
}

MainWindow::~MainWindow()
{
    delete context;
    delete graphBounds;
    delete param;
    delete ui;
}

void MainWindow::loadFileButton()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open CSV File", "C://csv", "CSV Files (*.csv)");
    if (!fileName.isEmpty())
    {
        doOperation(INIT_CONTEXT, context, param);
        param->fileName = strdup(fileName.toUtf8().constData());
        doOperation(READ_CSV_FILE, context, param);
        ui->fileNameLabel->setText(param->fileName);
        displaySummary();
        if (context->lineInfo->errorLines > ONE)
            displayTable(ALL_REGIONS);
        else if (context->lineInfo->errorLines == ONE)
            errorFileOpen();
    }
}

void MainWindow::loadDataButton()
{
    QString region = ui->regionComboBox->currentText();
    if (context->lineInfo->errorLines)
    {
        if (region == "All regions")
            displayTable(ALL_REGIONS);
        else if (!region.isEmpty())
        {
            strncpy(context->region, region.toUtf8().constData(), SIZE - ONE);
            context->region[SIZE - TWO] = '\0';
            doOperation(READ_DATA_FOR_REGION, context, param);
            displayTable(ONLY_REGION);
        }
    }
    else
        errorNoData();
}

void MainWindow::calculateButton()
{
    param->column = ui->columnComboBox->currentData().toInt();

    if (context->regionList->size > 0)
    {
        doOperation(CALCULATE, context, param);
        displayMetrics();
        drawGraph();
    }
    else
        errorLoadData();
}

void MainWindow::displayMetrics()
{
    ui->maxLine->setText(QString::number(context->metrics->max));
    ui->minLine->setText(QString::number(context->metrics->min));
    ui->medLine->setText(QString::number(context->metrics->med));
}

void MainWindow::displayTable(Status status)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(MAX_FIELD_COUNT);
    QStringList headers(QStringList() << "Year" << "Region" << "NPG" << "Birth Rate" << "Death Rate" << "GDW" << "Urbanization");
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    LinkedList* list = status ? context->regionList : context->dataList;
    int rowCount = list->size;
    ui->tableWidget->setRowCount(rowCount);
    if (list && rowCount > 0)
    {
        Iterator it = begin(list);
        for (int row = 0; row < rowCount && it.current != nullptr; ++row)
        {
            if (void* itemData = get(&it))
                updateTableRow(row, static_cast<DemographicData*>(itemData));
            next(&it);
        }
    }
}

void MainWindow::updateTableRow(int row, const DemographicData* data)
{
    ui->tableWidget->setItem(row, YEAR, new QTableWidgetItem(QString::number(data->year)));
    ui->tableWidget->setItem(row, REGION, new QTableWidgetItem(data->region));
    ui->tableWidget->setItem(row, NPG, new QTableWidgetItem(QString::number(data->npg)));
    ui->tableWidget->setItem(row, BIRTH_RATE, new QTableWidgetItem(QString::number(data->birthRate)));
    ui->tableWidget->setItem(row, DEATH_RATE, new QTableWidgetItem(QString::number(data->deathRate)));
    ui->tableWidget->setItem(row, GDW, new QTableWidgetItem(QString::number(data->gdw)));
    ui->tableWidget->setItem(row, URBANIZATION, new QTableWidgetItem(QString::number(data->urbanization)));
}

void MainWindow::displaySummary()
{
    QMessageBox::information(this, "File Summary",
        QString("Total Lines: %1\nValid Lines: %2\nError Lines: %3")
          .arg(context->dataList->size + context->lineInfo->errorLines)
          .arg(context->dataList->size)
          .arg(context->lineInfo->errorLines));
}

void MainWindow::updateRegionsComboBox()
{
    ui->regionComboBox->clear();
    QSet<QString> regions;

    Iterator it = begin(context->dataList);
    while (it.current != nullptr)
    {
        DemographicData* data = (DemographicData*)get(&it);
        regions.insert(QString::fromUtf8(data->region));
        next(&it);
    }

    ui->regionComboBox->addItem("All regions");
    for (const QString& region : regions)
        ui->regionComboBox->addItem(region);
    ui->regionComboBox->model()->sort(0);
}

void MainWindow::updateColumnsComboBox()
{
    ui->columnComboBox->clear();
    ui->columnComboBox->addItem("NPG", NPG);
    ui->columnComboBox->addItem("Birth Rate", BIRTH_RATE);
    ui->columnComboBox->addItem("Death Rate", DEATH_RATE);
    ui->columnComboBox->addItem("GDW", GDW);
    ui->columnComboBox->addItem("Urbanization", URBANIZATION);
}

void MainWindow::errorFileOpen()
{
    QMessageBox::warning(this, "File warning", "Incorrect or empty file");
}

void MainWindow::errorNoData()
{
    QMessageBox::warning(this, "No Data", "Please load CSV file first");
}

void MainWindow::errorLoadData()
{
    QMessageBox::warning(this, "No Data", "Please load region data first");
}

void MainWindow::disposeGraph()
{
    ui->graphLabel->clear();
}

void MainWindow::drawGraph()
{
    QPixmap pixmap(ui->graphLabel->size());
    QVector<QPointF> dataPoints;
    extractGraphData(dataPoints);

    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    calculateGraphBounds(dataPoints);
    setupGraphArea(painter, pixmap.size());
    drawAxes(painter, pixmap.size());

    QVector<QPointF> pixelPoints = calculatePointPositions(dataPoints, pixmap.size());
    drawPoints(painter, pixelPoints);
    drawLines(painter, pixelPoints);

    markExtremesAndMedian(painter, pixelPoints, dataPoints);
    addAxisLabels(painter, pixelPoints, dataPoints, pixmap.size());

    ui->graphLabel->setPixmap(pixmap);
}

void MainWindow::extractGraphData(QVector<QPointF>& points)
{
    Iterator it = begin(context->regionList);
    while (hasNext(&it))
    {
        DemographicData* data = (DemographicData*)get(&it);
        points.append(QPointF(data->year, getColumnValue(data, param->column)));
        next(&it);
    }
}

void MainWindow::calculateGraphBounds(QVector<QPointF>& points)
{
    graphBounds->x.min = graphBounds->x.max = points[0].x();
    graphBounds->y.min = graphBounds->y.max = points[0].y();

    for (const QPointF& p : points)
    {
        graphBounds->x.min = qMin(graphBounds->x.min, p.x());
        graphBounds->x.max = qMax(graphBounds->x.max, p.x());
        graphBounds->y.min = qMin(graphBounds->y.min, p.y());
        graphBounds->y.max = qMax(graphBounds->y.max, p.y());
    }
    graphBounds->paddingX = (graphBounds->x.max - graphBounds->x.min) * PADDING_COEFF;
    graphBounds->paddingY = (graphBounds->y.max - graphBounds->y.min) * PADDING_COEFF;
    graphBounds->x.min -= graphBounds->paddingX;
    graphBounds->x.max += graphBounds->paddingX;
    graphBounds->y.min -= graphBounds->paddingY;
    graphBounds->y.max += graphBounds->paddingY;
}

void MainWindow::setupGraphArea(QPainter& painter, const QSize& size)
{
    int width = size.width() - TWO * MARGIN;
    int height = size.height() - TWO * MARGIN;

    painter.setPen(Qt::black);
    painter.drawRect(MARGIN, MARGIN, width, height);
}

QVector<QPointF> MainWindow::calculatePointPositions(const QVector<QPointF>& dataPoints, const QSize& pixmapSize)
{
    QVector<QPointF> pixelPoints;
    for (const QPointF& p : dataPoints)
    {
        double x = MARGIN + (p.x() - graphBounds->x.min) / (graphBounds->x.max - graphBounds->x.min) * (pixmapSize.width() - TWO * MARGIN);
        double y = MARGIN + (ONE - (p.y() - graphBounds->y.min) / (graphBounds->y.max - graphBounds->y.min)) * (pixmapSize.height() - TWO * MARGIN);
        pixelPoints.append(QPointF(x, y));
    }

    return pixelPoints;
}

void MainWindow::drawPoints(QPainter& painter, const QVector<QPointF>& points)
{
    painter.setPen(Qt::black);
    for (const QPointF& point : points)
        painter.drawEllipse(point, TWO, TWO);
}

void MainWindow::drawLines(QPainter& painter, const QVector<QPointF>& points)
{
    painter.setPen(QPen(Qt::black, TWO));
    for (int i = 0; i < points.size() - ONE; ++i)
        painter.drawLine(points[i], points[i + ONE]);
}

int MainWindow::findClosestValueIndex(const QVector<QPointF>& dataPoints, double target)
{
    int closestIndex = -1;
    double closestDiff = INT_MAX;
    for (int i = 0; i < dataPoints.size(); ++i)
    {
        double diff = std::abs(dataPoints[i].y() - target);
        if (diff < closestDiff)
        {
            closestDiff = diff;
            closestIndex = i;
        }
    }

    return closestIndex;
}

void MainWindow::drawAxes(QPainter& painter, const QSize& size)
{
    painter.drawLine(MARGIN, MARGIN, MARGIN, size.height() - MARGIN);
    painter.drawLine(MARGIN, size.height() - MARGIN, size.width() - MARGIN, size.height() - MARGIN);
}

bool MainWindow::shouldDrawXLabel(const QVector<QPointF>& points, int index)
{
    return index > 0 && index < points.size() - 1 &&
           points[0].x() != points[index].x() &&
           points.last().x() != points[index].x();
}

void MainWindow::addAxisLabels(QPainter& painter, const QVector<QPointF>& pixelPoints, const QVector<QPointF>& dataPoints, const QSize& pixmapSize) {
    QFont font = painter.font();
    font.setPointSize(TEN);
    painter.setFont(font);
    painter.drawText(pixelPoints.first().x(), pixmapSize.height() - MARGIN / TWO, QString::number(dataPoints.first().x()));
    painter.drawText(pixelPoints.last().x(), pixmapSize.height() - MARGIN / TWO, QString::number(dataPoints.last().x()));
    auto maxIt = std::max_element(dataPoints.begin(), dataPoints.end(), [](const QPointF& a, const QPointF& b) { return a.y() < b.y(); });
    int maxIndex = std::distance(dataPoints.begin(), maxIt);
    double maxValue = dataPoints[maxIndex].y();
    auto minIt = std::min_element(dataPoints.begin(), dataPoints.end(), [](const QPointF& a, const QPointF& b) { return a.y() < b.y(); });
    int minIndex = std::distance(dataPoints.begin(), minIt);
    double minValue = dataPoints[minIndex].y();
    double medianValue = context->metrics->med;
    int medianIndex = findClosestValueIndex(dataPoints, medianValue);
    if (shouldDrawXLabel(dataPoints, maxIndex))
    {
        painter.drawText(QPointF(pixelPoints[maxIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(dataPoints[maxIndex].x()));
    }
    if (shouldDrawXLabel(dataPoints, minIndex)) {
        painter.drawText(QPointF(pixelPoints[minIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(dataPoints[minIndex].x()));
    }
    if (medianIndex != -1 && shouldDrawXLabel(dataPoints, medianIndex))
    {
        painter.drawText(QPointF(pixelPoints[medianIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(dataPoints[medianIndex].x()));
    }

    painter.drawText(QPointF(MARGIN - FOURTY, pixelPoints[maxIndex].y()), QString::number(maxValue));
    painter.drawText(QPointF(MARGIN - FOURTY, pixelPoints[minIndex].y()), QString::number(minValue));
    if (medianIndex != -1 && medianIndex != minIndex && medianIndex != maxIndex)
    {
        painter.drawText(QPointF(MARGIN - FOURTY, pixelPoints[medianIndex].y()),
                         QString::number(medianValue));
    }
}

bool MainWindow::compareYLess(const QPointF& a, const QPointF& b)
{
    return a.y() < b.y();
}

bool MainWindow::compareYGreater(const QPointF& a, const QPointF& b)
{
    return a.y() > b.y();
}

void MainWindow::markExtremesAndMedian(QPainter& painter, const QVector<QPointF>& pixelPoints, const QVector<QPointF>& dataPoints)
{
    QVector<QPointF>::const_iterator maxIt = std::max_element(dataPoints.begin(), dataPoints.end(), MainWindow::compareYLess);
    int maxIndex = std::distance(dataPoints.begin(), maxIt);

    auto minIt = std::min_element(dataPoints.begin(), dataPoints.end(), [](const QPointF& a, const QPointF& b) { return a.y() < b.y(); });
    int minIndex = std::distance(dataPoints.begin(), minIt);

    double medianValue = context->metrics->med;
    int medianIndex = findClosestValueIndex(dataPoints, medianValue);

    painter.setPen(QPen(Qt::blue, TWO));
    painter.drawEllipse(pixelPoints[maxIndex], THREE, THREE);
    painter.drawText(pixelPoints[maxIndex] + QPointF(FIVE, NEG_TEN), "Max");

    painter.drawEllipse(pixelPoints[minIndex], THREE, THREE);
    painter.drawText(pixelPoints[minIndex] + QPointF(FIVE, NEG_TEN), "Min");

    if (medianIndex != -1)
    {
        if (medianIndex == minIndex || medianIndex == maxIndex)
            painter.drawText(pixelPoints[medianIndex] + QPointF(FIVE, NEG_TWENTY), "Median");

        else
        {
            painter.drawEllipse(pixelPoints[medianIndex], THREE, THREE);
            painter.drawText(pixelPoints[medianIndex] + QPointF(FIVE, NEG_TEN), "Median");
        }
    }
}

