#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <camcapture.h>
#include <QFileDialog>
#include <QTimer>
#include <QPixmap>
#include <QDateTime>
#include <limits>

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

private:
    Ui::MainWindow *ui;
    bool connected = false;
    QPalette connectPal;
    QString filename();
    bool timerOnline = false;
    QTimer *timer = new QTimer(this);
    QTimer *live = new QTimer(this);
    qint64 prev = std::numeric_limits<qint64>::max();
    QDateTime time;
    QPixmap curr;

private slots:
    void liveShot();
    void singleShot();
    void connectPressed();
    void connectionResult(bool);
    void folderSelect();
    void setPath();
    void sequence();
    void accuire(int, int, char*, int, int);
};

#endif // MAINWINDOW_H
