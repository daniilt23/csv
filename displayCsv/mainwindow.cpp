#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , context(new AppContext{})
    , param(new AppParam)
    , fileinfo(new LineInfo)
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
    initContext(context);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::loadFileButton);
    connect(ui->loadDataButton, &QPushButton::clicked, this, &MainWindow::loadDataButton);
    connect(ui->calculateButton, &QPushButton::clicked, this, &MainWindow::calculateButton);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::updateRegionsComboBox);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::updateColumnsComboBox);
}

MainWindow::~MainWindow()
{
    delete fileinfo;
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
        clearContext(context);
        initContext(context);
        param->fileName = strdup(fileName.toUtf8().constData());
        doOperation(READ_CSV_FILE, context, param);
        ui->fileNameLabel->setText(param->fileName);
        displaySummary();
        if (context->lineInfo.errorLines > ONE)
            displayTable(ALL_REGIONS);
        else if (context->lineInfo.errorLines == ONE)
            errorFileOpen();
    }
}

void MainWindow::loadDataButton()
{
    QString region = ui->regionComboBox->currentText();
    if (context->lineInfo.errorLines)
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
    QString selectedColumn = ui->columnComboBox->currentText();

    if (selectedColumn == "NPG") param->column = TWO;
    else if (selectedColumn == "Birth Rate") param->column = THREE;
    else if (selectedColumn == "Death Rate") param->column = FOUR;
    else if (selectedColumn == "GDW") param->column = FIVE;
    else if (selectedColumn == "Urbanization") param->column = SIX;

    if (context->regionList.size > 0)
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
    ui->maxLine->setText(QString::number(context->metrics.max));
    ui->minLine->setText(QString::number(context->metrics.min));
    ui->medLine->setText(QString::number(context->metrics.med));
}

void MainWindow::displayTable(Status status)
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(MAX_FIELD_COUNT);
    QStringList headers;
    headers << "Year" << "Region" << "NPG" << "Birth Rate" << "Death Rate" << "GDW" << "Urbanization";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    LinkedList* list = status ? &context->regionList : &context->dataList;
    int rowCount = status ? context->regionList.size : context->dataList.size - context->lineInfo.errorLines;
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
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(data->year)));
    ui->tableWidget->setItem(row, ONE, new QTableWidgetItem(data->region));
    ui->tableWidget->setItem(row, TWO, new QTableWidgetItem(QString::number(data->npg)));
    ui->tableWidget->setItem(row, THREE, new QTableWidgetItem(QString::number(data->birthRate)));
    ui->tableWidget->setItem(row, FOUR, new QTableWidgetItem(QString::number(data->deathRate)));
    ui->tableWidget->setItem(row, FIVE, new QTableWidgetItem(QString::number(data->gdw)));
    ui->tableWidget->setItem(row, SIX, new QTableWidgetItem(QString::number(data->urbanization)));
}

void MainWindow::displaySummary()
{
    QMessageBox::information(this, "File Summary",
        QString("Total Lines: %1\nValid Lines: %2\nError Lines: %3")
          .arg(context->dataList.size)
          .arg(context->dataList.size - context->lineInfo.errorLines)
          .arg(context->lineInfo.errorLines));
}

void MainWindow::updateRegionsComboBox()
{
    ui->regionComboBox->clear();
    QSet<QString> regions;

    Iterator it = begin(&context->dataList);
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
    ui->columnComboBox->addItem("NPG");
    ui->columnComboBox->addItem("Birth Rate");
    ui->columnComboBox->addItem("Death Rate");
    ui->columnComboBox->addItem("GDW");
    ui->columnComboBox->addItem("Urbanization");
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
    QVector<double> years, values;
    extractGraphData(years, values);

    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    calculateGraphBounds(years, values);
    setupGraphArea(painter, pixmap.size());

    drawAxes(painter, pixmap.size());

    QVector<QPointF> points = calculatePointPositions(years, values, pixmap.size());
    drawDataPoints(painter, points);
    drawDataLines(painter, points);

    markExtremesAndMedian(painter, points, values);

    addAxisLabels(painter, points, years, values, pixmap.size());

    ui->graphLabel->setPixmap(pixmap);
}

void MainWindow::extractGraphData(QVector<double>& years, QVector<double>& values)
{
    Iterator it = begin(&context->regionList);
    while (it.current != 0)
    {
        DemographicData* data = (DemographicData*)get(&it);
        years.append(data->year);
        values.append(getValueForCurrentColumn(data));
        next(&it);
    }
}

double MainWindow::getValueForCurrentColumn(const DemographicData* data)
{
    double value = NAN;
    switch (param->column)
    {
    case YEAR:
        value = (float)data->year;
        break;
    case NPG:
        value = data->npg;
        break;
    case BIRTH_RATE:
        value = data->birthRate;
        break;
    case DEATH_RATE:
        value = data->deathRate;
        break;
    case GDW:
        value = data->gdw;
        break;
    case URBANIZATION:
        value = data->urbanization;
        break;
    }

    return value;
}

void MainWindow::calculateGraphBounds(const QVector<double>& years, const QVector<double>& values)
{
    graphBounds->minX = *std::min_element(years.begin(), years.end());
    graphBounds->maxX = *std::max_element(years.begin(), years.end());
    graphBounds->minY = *std::min_element(values.begin(), values.end());
    graphBounds->maxY = *std::max_element(values.begin(), values.end());

    graphBounds->paddingX = (graphBounds->maxX - graphBounds->minX) * PADDING_COEFF;
    graphBounds->paddingY = (graphBounds->maxY - graphBounds->minY) * PADDING_COEFF;

    graphBounds->minX -= graphBounds->paddingX;
    graphBounds->maxX += graphBounds->paddingX;
    graphBounds->minY -= graphBounds->paddingY;
    graphBounds->maxY += graphBounds->paddingY;
}

void MainWindow::setupGraphArea(QPainter& painter, const QSize& size)
{
    int width = size.width() - TWO * MARGIN;
    int height = size.height() - TWO * MARGIN;

    painter.setPen(Qt::black);
    painter.drawRect(MARGIN, MARGIN, width, height);
}

QVector<QPointF> MainWindow::calculatePointPositions(const QVector<double>& years, const QVector<double>& values,
                                                     const QSize& pixmapSize)
{
    QVector<QPointF> points;

    for (int i = 0; i < years.size(); ++i)
    {
        double x = MARGIN + (years[i] - graphBounds->minX) / (graphBounds->maxX - graphBounds->minX) * (pixmapSize.width() - TWO * MARGIN);
        double y = MARGIN + (ONE - (values[i] - graphBounds->minY) / (graphBounds->maxY - graphBounds->minY)) * (pixmapSize.height() - TWO * MARGIN);
        points.append(QPointF(x, y));
    }

    return points;
}

void MainWindow::drawDataPoints(QPainter& painter, const QVector<QPointF>& points)
{
    painter.setPen(Qt::black);
    for (const QPointF& point : points)
        painter.drawEllipse(point, TWO, TWO);
}

void MainWindow::drawDataLines(QPainter& painter, const QVector<QPointF>& points)
{
    painter.setPen(QPen(Qt::black, TWO));
    for (int i = 0; i < points.size() - ONE; ++i)
        painter.drawLine(points[i], points[i + ONE]);
}

void MainWindow::markExtremesAndMedian(QPainter& painter, const QVector<QPointF>& points, const QVector<double>& values)
{
    auto maxIt = std::max_element(values.begin(), values.end());
    int maxIndex = std::distance(values.begin(), maxIt);


    auto minIt = std::min_element(values.begin(), values.end());
    int minIndex = std::distance(values.begin(), minIt);

    double medianValue = context->metrics.med;
    int medianIndex = findClosestValueIndex(values, medianValue);

    painter.setPen(QPen(Qt::blue, TWO));

    painter.drawEllipse(points[maxIndex], THREE, THREE);
    painter.drawText(points[maxIndex] + QPointF(FIVE, NEG_TEN), "Max");

    painter.drawEllipse(points[minIndex], THREE, THREE);
    painter.drawText(points[minIndex] + QPointF(FIVE, NEG_TEN), "Min");

    if (medianIndex != -1)
    {
        if (medianIndex == minIndex || medianIndex == minIndex) {
            painter.drawText(points[medianIndex] + QPointF(FIVE, NEG_TWENTY), "Median");
        }
        else
        {
            painter.drawEllipse(points[medianIndex], THREE, THREE);
            painter.drawText(points[medianIndex] + QPointF(FIVE, NEG_TEN), "Median");
        }
    }
}

int MainWindow::findClosestValueIndex(const QVector<double>& values, double target)
{
    int closestIndex = -1;
    double closestDifference = std::numeric_limits<double>::max();
    for (int i = 0; i < values.size(); ++i)
    {
        double currentDifference = std::abs(values[i] - target);
        if (currentDifference < closestDifference)
        {
            closestDifference = currentDifference;
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
    return index > 0 && index < points.size() - ONE &&
           points[0].x() != points[index].x() &&
           points.last().x() != points[index].x();
}

void MainWindow::addAxisLabels(QPainter& painter, const QVector<QPointF>& points,
    const QVector<double>& years, const QVector<double>& values,
    const QSize& pixmapSize)
{
    QFont font = painter.font();
    font.setPointSize(TEN);
    painter.setFont(font);

    painter.drawText(points[0].x(), pixmapSize.height() - MARGIN / TWO, QString::number(years[0]));
    painter.drawText(points.last().x(), pixmapSize.height() - MARGIN / TWO, QString::number(years.last()));

    auto maxIt = std::max_element(values.begin(), values.end());
    int maxIndex = std::distance(values.begin(), maxIt);
    auto minIt = std::min_element(values.begin(), values.end());
    int minIndex = std::distance(values.begin(), minIt);
    double medianValue = context->metrics.med;
    int medianIndex = findClosestValueIndex(values, medianValue);

    if (shouldDrawXLabel(points, maxIndex))
    {
        painter.drawText(QPointF(points[maxIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(years[maxIndex]));
    }
    if (shouldDrawXLabel(points, minIndex))
    {
        painter.drawText(QPointF(points[minIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(years[minIndex]));
    }
    if (medianIndex != -1 && shouldDrawXLabel(points, medianIndex))
    {
        painter.drawText(QPointF(points[medianIndex].x(), pixmapSize.height() - MARGIN / TWO),
                         QString::number(years[medianIndex]));
    }

    painter.drawText(QPointF(MARGIN - FOURTY, points[maxIndex].y()), QString::number(*maxIt));
    painter.drawText(QPointF(MARGIN - FOURTY, points[minIndex].y()), QString::number(*minIt));
    if (medianIndex != -1 && minIndex != medianIndex && maxIndex != medianIndex)
    {
        painter.drawText(QPointF(MARGIN - FOURTY, points[medianIndex].y()),
                         QString::number(medianValue));
    }
}



