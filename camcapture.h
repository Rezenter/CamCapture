#ifndef CAMCAPTURE_H
#define CAMCAPTURE_H
//#include <apps/Common/exampleHelper.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#ifdef _WIN32
#include <mvDisplay/Include/mvIMPACT_acquire_display.h>
using namespace mvIMPACT::acquire::display;
#endif // #ifdef _WIN32
using namespace mvIMPACT::acquire;
#include <QList>
#include <QDebug>
#include <QObject>

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
    void captured(int, int, char*, int, int); //send image width, height, pointer to char array representing pixels
    void connectionResult(bool);

private:
    QString path;
    int x = 0;
    int y = 0;
    int pitch = 0;
    int bpp = 0;
    char* copy;
    int SaveBMP(QString, const char*, int, int, int, int );
    bool connected = false; //if currently connected to the device
    int currN = 0; //current device position in "devNames" and "devMgr"
    DeviceManager devMgr; //device holder
    int devCount = 0; //total device count
    QList<QString> devNames;
    Device* curr;
    FunctionInterface* fi;
    const int iMaxWaitTime_ms = 100;// Define the Image Result Timeout (The maximum time allowed for the Application
                                     // to wait for a Result). Infinity value:-1
                                     // USB 1.1 on an embedded system needs a large timeout for the first image.
};

#endif // CAMCAPTURE_H
