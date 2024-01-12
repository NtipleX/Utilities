#include "mainwindow.h"
#include "QScreen.h"
#include "QTableView.h"
#include <QAbstractItemModel>
#include "cmptablemodel.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include <QDebug>
#include <QBuffer>
#include <QtMath>
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    QPixmap originalPixmap = makeScreenshot();

    CmpTableModel* model = new CmpTableModel(originalPixmap);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setModel(model);

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("database.db");
    m_database.open();
    QObject::connect(&m_timer, &QTimer::timeout, this, &MainWindow::buttonClicked);
    m_timer.setInterval(60000);
    fetchData();
}

MainWindow::~MainWindow()
{
    QSqlDatabase::database().close();
    m_workerThread.quit();
    QObject::connect(&m_workerThread, &QThread::finished, &m_workerThread, &QObject::deleteLater);
    delete ui;
}

void MainWindow::buttonClicked()
{
    if (!m_database.open())
    {
        qWarning("%s", m_database.lastError().text().toLocal8Bit().data());
        return;
    }
    ui->pushButton->setDisabled(true);
    m_timer.start();

    /// Capture screenshot and scale it
    originalPixmap = makeScreenshot();
    QImage toPrevious = previousScreen;
    QImage toCompare = scaleToCompare(originalPixmap);
    previousScreen = toCompare;
    if(toPrevious.isNull())
        toPrevious = toCompare;

    /// Run computation of previous and current screenshots
    m_worker = new Worker(toCompare, toPrevious);
    m_worker->moveToThread(&m_workerThread);
    QObject::connect(&m_workerThread, &QThread::started, m_worker, &Worker::runFunction);
    QObject::connect(m_worker, &Worker::resultReady, this, &MainWindow::postComputation);
    m_workerThread.start();
}

void MainWindow::fetchData()
{
    QSqlQuery query;
    if(!query.exec("SELECT id, screen, percentage FROM ScreenCmp ORDER BY id DESC LIMIT 7"))
    {
        qWarning("%s", query.lastError().text().toLocal8Bit().data());
        return;
    }
    QVector<QPair<QByteArray, ushort>> dataCaptures;
    while(query.next())
    {
        QByteArray screenshot = query.value("screen").toByteArray();
        QPixmap loaded;
        loaded.loadFromData(screenshot);
        if(previousScreen.isNull())
            previousScreen = scaleToCompare(loaded);
        ushort percent = query.value("percentage").toInt();
        dataCaptures.append(qMakePair<QByteArray, ushort>(screenshot, percent));
    }

    CmpTableModel* dataModel = qobject_cast<CmpTableModel*>(ui->tableView->model());
    if(dataModel)
    {
        dataModel->updateCaptures(dataCaptures);
        ui->tableView->update();
    }
}

QImage MainWindow::scaleToCompare(const QPixmap& screen)
{
    const int scaledPixelAmount = 256;

    if(screen.isNull())
        return QImage();

    QSize partition = screen.size()/scaledPixelAmount;
    QSize scaled(screen.width()/partition.width(),
                 screen.height()/(partition.height()));
    return screen.scaled(scaled).toImage();
}

QPixmap MainWindow::makeScreenshot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if(!screen)
        return QPixmap();
    return screen->grabWindow(0);
}

void MainWindow::postComputation(float percentResult)
{
    qDebug("%.2f", percentResult);
    delete m_worker;
    m_workerThread.quit();
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::ReadWrite);
    originalPixmap.save(&buffer, "PNG");

    QSqlQuery databaseQuery(m_database);
    databaseQuery.exec("CREATE TABLE IF NOT EXISTS ScreenCmp (id integer primary key autoincrement, screen BLOB, percentage integer)");
    databaseQuery.prepare("INSERT INTO ScreenCmp (screen, percentage) VALUES (:scr, :per)");
    databaseQuery.bindValue(":scr", QVariant(bytes));
    databaseQuery.bindValue(":per", roundf(percentResult));

    if (!databaseQuery.exec())
    {
        qWarning("%s", databaseQuery.lastError().text().toLocal8Bit().data());
        return;
    }
    fetchData();
}

double Worker::computeResult()
{
    /// How different each pixel is to previous based on rgb values - effects overall percentage
    double percentResult = 100.f;
    double partPercent = percentResult/(m_first.size().width()*m_first.size().height());
    const double maxPercentAffectPerPixel = qPow(255, 2)*3; //rgb

    for(int i=0; i<m_first.size().width(); i++)
    {
        for(int j=0; j<m_first.size().height(); j++)
        {
            QColor col = m_first.pixelColor(i, j);
            QColor pcol = m_second.pixelColor(i, j);
            double diff =
                qPow((col.red() - pcol.red()), 2) +
                qPow((col.blue() - pcol.blue()), 2) +
                qPow((col.green() - pcol.green()), 2);
            diff = partPercent/(maxPercentAffectPerPixel/(diff+1));
            percentResult -= diff;
        }
    }
    return percentResult;
}

void Worker::runFunction()
{
    double result = computeResult();
    emit resultReady(result);
}

Worker::Worker(QImage first, QImage second): m_first(first), m_second(second)
{}
