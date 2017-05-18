#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectPal = ui->connectButton->palette();
    connectionResult(false);
    QObject::connect(ui->connectButton, &QPushButton::pressed, this, &MainWindow::connectPressed);
    QObject::connect(&cc, SIGNAL(connectionResult(bool)), this, SLOT(connectionResult(bool)));
    QObject::connect(this, SIGNAL(connect()), &cc, SLOT(connect()));
    QObject::connect(this, SIGNAL(disconnect()), &cc, SLOT(disconnect()));
    QObject::connect(ui->folderButton, &QPushButton::pressed, this, &MainWindow::folderSelect);
    QObject::connect(ui->pathLine, &QLineEdit::editingFinished, this, &MainWindow::setPath);
    QObject::connect(ui->shotButton, &QPushButton::pressed, this, &MainWindow::singleShot);
    QObject::connect(this, SIGNAL(capture(QString)), &cc, SLOT(capture(QString)));
    QObject::connect(this, SIGNAL(savePath(QString)), &cc, SLOT(setPath(QString)));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(singleShot()));
    QObject::connect(live, SIGNAL(timeout()), this, SLOT(liveShot()));
    QObject::connect(ui->seqButton, &QPushButton::pressed, this, &MainWindow::sequence);
    QObject::connect(&cc, SIGNAL(captured(int, int, char*, int, int)), this, SLOT(accuire(int, int, char*, int, int)));
    emit savePath(ui->pathLine->text());
}

MainWindow::~MainWindow()
{
    timer->stop();
    delete timer;
    live->stop();
    //delete live; //wtf
    emit disconnect();
    delete ui;
}

void MainWindow::connectionResult(bool res){
    connected = res;
    ui->seqButton->setEnabled(connected);
    ui->shotButton->setEnabled(connected);
    if(connected){
        live->start(200);
        connectPal.setColor(QPalette::Button, QColor(Qt::green));
        ui->connectButton->setAutoFillBackground(true);
        ui->connectButton->setPalette(connectPal);
        ui->connectButton->update();
    }else{
        live->stop();
        connectPal.setColor(QPalette::Button, QColor(Qt::red));
        ui->connectButton->setAutoFillBackground(true);
        ui->connectButton->setPalette(connectPal);
        ui->connectButton->update();
    }
}

void MainWindow::connectPressed(){
    if(connected){
        emit disconnect();
    }else{
        emit connect();
    }
}

void MainWindow::folderSelect(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select save directory"),
                                                    ui->pathLine->text(),
                                                     QFileDialog::DontResolveSymlinks);
    if(dir.length() != 0){
        ui->pathLine->setText(dir);
        setPath();
    }
}

void MainWindow::setPath(){
    emit savePath(ui->pathLine->text());
}

void MainWindow::singleShot(){
    emit capture(filename());
    ui->shotBox->setValue(ui->shotBox->value() + 1);
}

QString MainWindow::filename(){
    QString res = QString("%1").arg(ui->seriesBox->value(), 2, 10, QChar('0'));
    res += "_";
    res += QString("%1").arg(ui->shotBox->value(), 4, 10, QChar('0'));
    return res;
}

void MainWindow::sequence(){
    if(timerOnline){
        timer->stop();
        ui->shotButton->setEnabled(true);
        ui->seqBox->setEnabled(true);
        ui->shotBox->setEnabled(true);
        ui->seriesBox->setEnabled(true);
        timerOnline = false;
    }else{
        timerOnline = true;
        singleShot();
        ui->shotButton->setEnabled(false);
        ui->seqBox->setEnabled(false);
        ui->shotBox->setEnabled(false);
        ui->seriesBox->setEnabled(false);
        timer->start(1000*ui->seqBox->value());
    }
}

void MainWindow::liveShot(){
    emit capture("");
}

void MainWindow::accuire(int x, int y, char* begin, int pitch, int bitsPerPixel){
    static const quint16 PALETTE_ENTRIES = 256;
    if( begin )
    {
        QByteArray data;

            BITMAPINFOHEADER    bih;
            BITMAPFILEHEADER    bfh;
            quint16             linelen = ( ( x * bitsPerPixel + 31 ) / 32 * 4 );  // DWORD aligned
            int                 YPos;
            int                 YStart = 0;
            memset( &bfh, 0, sizeof( BITMAPFILEHEADER ) );
            memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
            bfh.bfType          = 0x4d42;
            bfh.bfSize          = sizeof( bih ) + sizeof( bfh ) + sizeof( RGBQUAD ) * PALETTE_ENTRIES + static_cast<LONG>( linelen ) * static_cast<LONG>( y );
            bfh.bfOffBits       = sizeof( bih ) + sizeof( bfh ) + sizeof( RGBQUAD ) * PALETTE_ENTRIES;
            bih.biSize          = sizeof( bih );
            bih.biWidth         = x;
            bih.biHeight        = y;
            bih.biPlanes        = 1;
            bih.biBitCount      = static_cast<quint16>( bitsPerPixel );
            bih.biSizeImage     = static_cast<quint32>( linelen ) * static_cast<quint32>( y );

            data.append((char *)&bfh, sizeof( bfh ));
            data.append((char *)&bih, sizeof( bih ));

            RGBQUAD rgbQ;
            for( int i = 0; i < PALETTE_ENTRIES; i++ ){
                rgbQ.rgbRed      = static_cast<quint8>( i );
                rgbQ.rgbGreen    = static_cast<quint8>( i );
                rgbQ.rgbBlue     = static_cast<quint8>( i );
                rgbQ.rgbReserved = static_cast<quint8>( 0 );
                data.append((char *)&rgbQ, sizeof(rgbQ));
            }
            for( YPos = YStart + y - 1; YPos >= YStart; YPos-- ){
                data.append(&begin[YPos * pitch], linelen);
            }
            ui->fpsLabel->setText(QString::number(1000/(time.currentMSecsSinceEpoch() - prev)));
            prev = time.currentMSecsSinceEpoch();
            curr.loadFromData(data, "BMP");
            ui->liveLable->setPixmap(curr);
    }
}
