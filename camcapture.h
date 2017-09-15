#ifndef CAMCAPTURE_H
#define CAMCAPTURE_H
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#ifdef _WIN32
#include <mvDisplay/Include/mvIMPACT_acquire_display.h>
using namespace mvIMPACT::acquire::display;
#endif // #ifdef _WIN32
using namespace mvIMPACT::acquire;
#include <QList>
#include <QDebug>
#include <QObject>
#include <QPixmap>

class CamCapture : public QObject{

    Q_OBJECT

public:
    CamCapture(QObject *parent = 0);

public slots:
    void reqDevList(); //request "devList", emits "sendDevList" signal
    void setN(int N); //set as current device at position "N" in "devNames" and connect
    void capture(QString filename = "");
    void connect();
    void disconnect();
    void setPath(QString);

signals:
    void sendDevList(QList<QString>); //send "devList"
    void captured(QPixmap); //send image in QPixmap format
    void connectionResult(bool);
    void saved();

private:
    static const quint16 PALETTE_ENTRIES = 256;
    QString path;
    int x = 0;
    int y = 0;
    int pitch = 0;
    int bpp = 0;
    char* copy;
    bool connected = false; //if currently connected to the device
    int currN = 0; //current device position in "devNames" and "devMgr"
    DeviceManager devMgr; //device holder
    int devCount = 0; //total device count
    QList<QString> devNames;
    Device* curr;
    FunctionInterface* fi;
    QPixmap pixMap;
    const int iMaxWaitTime_ms = 30;// Define the Image Result Timeout (The maximum time allowed for the Application
                                     // to wait for a Result). Infinity value:-1
                                     // USB 1.1 on an embedded system needs a large timeout for the first image.
};

#endif // CAMCAPTURE_H
