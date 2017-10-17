#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <camcapture.h>
#include <QFileDialog>
#include <QTimer>
#include <QPixmap>
#include <QDateTime>
#include <limits>
#include <QThread>
#include "comchatter.h"
#include "logger.h"
#include <QCoreApplication>
#include <QSettings>
#include <QBrush>
#include <QPen>
#include <QMouseEvent>
#include <QPainter>
#include <QElapsedTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    CamCapture cc;

signals:
    void disconnect();
    void connect();
    void savePath(QString);
    void capture(QString);
    void comConnect(QString);
    void close();
    void send(QString);

private:
    Ui::MainWindow *ui;
    bool connected = false;
    QPalette connectPal;
    QString filename();
    bool timerOnline = false;
    QTimer *timer = new QTimer(this);
    QTimer *live = new QTimer(this);
    qint64 prev[5];
    int prevIndex = 0;
    QDateTime time;
    QPixmap curr;
    QString path = QCoreApplication::applicationDirPath();
    bool opened = false;
    QThread *comThread;
    ComChatter *port;
    Logger *log;
    QString portName;
    QSettings *settings;
    int pos;
    void setControlsEnabled(bool);
    int d2s(double);
    double s2d(int);
    bool scanning = false;
    double coeff;
    QString inBuf;
    void newPos(double);
    bool seq = false;
    QDateTime now;
    QString lastDir = "";
    QSettings *param;
    QPen pen;
    QBrush brush;
    void mouseMoveEvent(QMouseEvent *);
    int radius;
    qreal eps;
    void saveSettings();
    QPoint center;
    int prevPos;
    QElapsedTimer fromBegin;
    bool DEBUG = false;
    bool LOG = false;

private slots:
    void liveShot();
    void singleShot();
    void connectPressed();
    void connectionResult(bool);
    void folderSelect();
    void setPath();
    void sequence();
    void accuire(QPixmap);
    void editing();
    void edited();

public slots:
    void saved();
    void closed();
    void error(QString);
    void in(QString);
    void motConnect();
    void stopButton();
    void l();
    void r();
    void l10();
    void r10();
    void scan();
    void setPos();
    void stop();
};

#endif // MAINWINDOW_H
