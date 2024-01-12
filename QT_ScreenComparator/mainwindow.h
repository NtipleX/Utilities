#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void buttonClicked();
    void postComputation(float percentResult);

private:
    void fetchData();
    QImage scaleToCompare(const QPixmap& screen);
    QPixmap makeScreenshot();

    QSqlDatabase m_database;
    QPixmap originalPixmap;
    QImage previousScreen;
    class Worker* m_worker;
    QThread m_workerThread;
    QTimer m_timer;

    Ui::MainWindow *ui;
};


class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QImage first, QImage second);

public slots:
    void runFunction();

signals:
    void resultReady(int result);

private:
    QImage m_first;
    QImage m_second;
    double computeResult();
};

#endif // MAINWINDOW_H
