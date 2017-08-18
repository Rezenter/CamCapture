#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Cam & motor control");
    settings = new QSettings(path + "/settings.ini", QSettings::IniFormat);
    coeff = settings->value("stepsInDegree", 20).toDouble();
    log = new Logger();
    log->print(path);
    comThread = new QThread();
    port = new ComChatter();
    port->moveToThread(comThread);
    port->port.moveToThread(comThread);
    connectPal = ui->connectButton->palette();
    connectionResult(false);
    QObject::connect(port, SIGNAL(dead()), comThread, SLOT(quit()));
    QObject::connect(comThread, SIGNAL(finished()), port, SLOT(deleteLater()));
    QObject::connect(port, SIGNAL(dead()), comThread, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(comConnect(QString)), port, SLOT(connect(QString)));
    QObject::connect(this, SIGNAL(close()), port, SLOT(close()));
    QObject::connect(this, SIGNAL(send(QString)), port, SLOT(send(QString)));
    QObject::connect(port, SIGNAL(closed()), this, SLOT(closed()));
    QObject::connect(port, SIGNAL(error(QString)), this, SLOT(error(QString)));
    QObject::connect(port, SIGNAL(out(QString)), this, SLOT(in(QString)));
    QObject::connect(port, SIGNAL(error(QString)), log, SLOT(error(QString)));
    QObject::connect(port, SIGNAL(out(QString)), log, SLOT(received(QString)));
    QObject::connect(this, SIGNAL(send(QString)), log, SLOT(print(QString)));
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
    QObject::connect(ui->motButton, &QPushButton::pressed, this, &MainWindow::motConnect);
    QObject::connect(ui->lButton, &QPushButton::pressed, this, &MainWindow::l);
    QObject::connect(ui->rButton, &QPushButton::pressed, this, &MainWindow::r);
    QObject::connect(ui->l10Button, &QPushButton::pressed, this, &MainWindow::l10);
    QObject::connect(ui->r10Button, &QPushButton::pressed, this, &MainWindow::r10);
    QObject::connect(ui->posSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::setPos);
    QObject::connect(ui->thetaButton, &QPushButton::pressed, this, &MainWindow::scan);
    QObject::connect(ui->pathLine, &QLineEdit::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->seriesBox, &QSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->shotBox, &QSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->seqBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->thetaStepSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->fromSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->toSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->posSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->pathLine, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textEdited), this, &MainWindow::editing);
    QObject::connect(ui->seriesBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->shotBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->seqBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->thetaStepSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->fromSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->toSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->posSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->stopButton, &QPushButton::pressed, this, &MainWindow::stop);
    emit savePath(ui->pathLine->text());
    setControlsEnabled(false);
    comThread->start();
}

MainWindow::~MainWindow()
{
    comThread->quit();
    comThread->requestInterruption();
    log->print("QThread dead ? : ");
    if(comThread->wait()){
        log->print("True");
    }else{
        log->print("False");
    }
    delete comThread;
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
        if(opened){
            ui->thetaButton->setEnabled(true);
        }
    }else{
        live->stop();
        connectPal.setColor(QPalette::Button, QColor(Qt::red));
        ui->connectButton->setAutoFillBackground(true);
        ui->connectButton->setPalette(connectPal);
        ui->connectButton->update();
        ui->thetaButton->setEnabled(false);
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
    QString dir = ui->pathLine->text() + '/' + QString("%1").arg(ui->seriesBox->value(), 2, 10, QChar('0'));
    if(!QDir(dir).exists() || lastDir != dir){
        lastDir = dir;
        QDir().mkdir(dir);
        out = new QFile(dir + "/info.txt");
        out->open(QIODevice::WriteOnly);
        outStream = new QTextStream(out);
        *outStream << now.currentDateTime().toString("ddd MMMM d yyyy") << " @ " << QString::number(ui->posSpinBox->value()) << "°" << endl;
    }
    emit savePath(dir);
    *outStream << QString("%1").arg(ui->shotBox->value(), 4, 10, QChar('0')) << " " << now.currentDateTime().toString("hh:mm:ss") << " @ " << QString::number(ui->posSpinBox->value()) << "°";
    if(seq){
        *outStream << " Time Sequence, time step = " << QString::number(ui->seqBox->value());
    }else if(scanning){
        *outStream << " Theta scan, theta step = " << QString::number(ui->thetaStepSpinBox->value());
    }else{
        *outStream << " Single shot";
    }
    *outStream << endl;
    QString res = QString("%1").arg(ui->seriesBox->value(), 2, 10, QChar('0'));
    res += "_";
    res += QString("%1").arg(ui->shotBox->value(), 4, 10, QChar('0'));
    return res;
}

void MainWindow::sequence(){
    if(timerOnline){
        seq = false;
        timer->stop();
        ui->shotButton->setEnabled(true);
        ui->seqBox->setEnabled(true);
        ui->shotBox->setEnabled(true);
        ui->seriesBox->setEnabled(true);
        ui->thetaButton->setEnabled(true);
        timerOnline = false;
    }else{
        timerOnline = true;
        seq = true;
        singleShot();
        ui->shotButton->setEnabled(false);
        ui->seqBox->setEnabled(false);
        ui->shotBox->setEnabled(false);
        ui->seriesBox->setEnabled(false);
        ui->thetaButton->setEnabled(false);
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
            ui->radioButton->setChecked(!ui->radioButton->isChecked());
    }
}

void MainWindow::closed(){
    opened = false;
    scanning = false;
    setControlsEnabled(false);
    log->print("port closed");
}

void MainWindow::error(QString data){
    if(QString::compare("No such file or directory", data, Qt::CaseSensitive) == 0){
        data = "No such port, file or directory. I/O open error.";
    }
    qDebug() << "error: " << data;
}

void MainWindow::in(QString data){
    inBuf += data;
    if(inBuf.endsWith("\r\n")){
        inBuf.chop(2);
        if(inBuf.endsWith("connected", Qt::CaseInsensitive)){
             opened = true;
             setControlsEnabled(true);
             if(!connected){
                 ui->thetaButton->setEnabled(false);
             }
        }else{
            pos = inBuf.toInt();
            ui->posSpinBox->setValue(s2d(pos));
            ui->posSpinBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
            if(scanning){
                scan();
            }
        }
        inBuf = "";
    }
}

void MainWindow::motConnect(){
    emit close();
    timer->stop();
    if(opened){
        opened = false;
        scanning = false;
    }else{
        log->print("connecting to " + settings->value("portName", "COM1").toString());
        emit comConnect(settings->value("portName", "COM1").toString());
        pos = 0;
        ui->posSpinBox->setValue(0);
    }
}

void MainWindow::setControlsEnabled(bool b){
    ui->l10Button->setEnabled(b);
    ui->r10Button->setEnabled(b);
    ui->lButton->setEnabled(b);
    ui->rButton->setEnabled(b);
    ui->posSpinBox->setEnabled(b);
    ui->thetaButton->setEnabled(b);
    ui->stopButton->setEnabled(b);
    if(b){
        ui->motButton->setStyleSheet("QPushButton { background-color : green;}");
    }else{
        ui->motButton->setStyleSheet("QPushButton { background-color : red;}");
    }
    if(scanning){
        ui->motButton->setStyleSheet("QPushButton { background-color : yellow;}");
        ui->stopButton->setEnabled(true);
    }
}

void MainWindow::stopButton(){
    emit send("0");
}

void MainWindow::l(){
    emit send(QString::number(d2s(-1)));
}

void MainWindow::r(){
    emit send(QString::number(d2s(1)));
}

void MainWindow::l10(){
    emit send(QString::number(d2s(-10)));
}

void MainWindow::r10(){
    emit send(QString::number(d2s(10)));
}

void MainWindow::newPos(double d){
    emit send(QString::number(d2s(d) - pos));
}

void MainWindow::setPos(){
    newPos(ui->posSpinBox->value());
}

void MainWindow::scan(){
    if(!scanning && ui->fromSpinBox->value() != ui->toSpinBox->value()){
        newPos(ui->fromSpinBox->value());
        scanning = true;
        setControlsEnabled(false);
        ui->seqButton->setEnabled(false);
    }else{
        singleShot();
        if(ui->fromSpinBox->value() < ui->toSpinBox->value()){
            emit send(QString::number(d2s(ui->thetaStepSpinBox->value())));
        }else if(ui->fromSpinBox->value() > ui->toSpinBox->value()){
            emit send(QString::number(d2s(-ui->thetaStepSpinBox->value())));
        }
        if(ui->toSpinBox->value() == s2d(pos) ||
                ((ui->fromSpinBox->value() < ui->toSpinBox->value()) && ui->toSpinBox->value() <= s2d(pos)) ||
                ((ui->fromSpinBox->value() > ui->toSpinBox->value()) && ui->toSpinBox->value() >= s2d(pos))){
            singleShot();
            scanning = false;
            setControlsEnabled(true);
            ui->seqButton->setEnabled(true);
        }
    }
}

int MainWindow::d2s(double d){
    return (int)(d*coeff);
}

double MainWindow::s2d(int s){
    return (double)(s/coeff);
}

void MainWindow::editing(){
    QObject* from = sender();
    if(from == ui->pathLine){
        ui->pathLine->setStyleSheet("QLineEdit { background-color : yellow; }");
    }else if(from == ui->seriesBox){
        ui->seriesBox->setStyleSheet("QSpinBox { background-color : yellow; }");
    }else if(from == ui->seqBox){
        ui->seqBox->setStyleSheet("QDoubleSpinBox { background-color : yellow; }");
    }else if(from == ui->thetaStepSpinBox){
        ui->thetaStepSpinBox->setStyleSheet("QDoubleSpinBox { background-color : yellow; }");
    }else if(from == ui->fromSpinBox){
        ui->fromSpinBox->setStyleSheet("QDoubleSpinBox { background-color : yellow; }");
    }else if(from == ui->toSpinBox){
        ui->toSpinBox->setStyleSheet("QDoubleSpinBox { background-color : yellow; }");
    }else if(from == ui->posSpinBox){
        ui->posSpinBox->setStyleSheet("QDoubleSpinBox { background-color : yellow; }");
    }
}

void MainWindow::edited(){
    QObject* from = sender();
    if(from == ui->pathLine){
        ui->pathLine->setStyleSheet("QLineEdit { background-color : white; }");
        ui->seriesBox->setValue(0);
        ui->shotBox->setValue(0);
    }else if(from == ui->seriesBox){
        ui->seriesBox->setStyleSheet("QSpinBox { background-color : white; }");
        ui->shotBox->setValue(0);
    }else if(from == ui->seqBox){
        ui->seqBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
    }else if(from == ui->thetaStepSpinBox){
        ui->thetaStepSpinBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
    }else if(from == ui->fromSpinBox){
        ui->fromSpinBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
    }else if(from == ui->toSpinBox){
        ui->toSpinBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
    }else if(from == ui->posSpinBox){
        ui->posSpinBox->setStyleSheet("QDoubleSpinBox { background-color : white; }");
    }
}

void MainWindow::stop(){
    emit send(QString::number(0));
}
