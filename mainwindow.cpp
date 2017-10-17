#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings = new QSettings(path + "/settings.ini", QSettings::IniFormat);
    LOG = settings->value("logging", false).toBool();
    if(LOG){
        DEBUG = settings->value("debug", false).toBool();
        log = new Logger();
        log->print(path);
    }
    if(DEBUG){
        log->print("debug");
    }
    resize(settings->value("windowSize", QSize(800, 600)).toSize());
    move(settings->value("windowPos", QPoint(0, 0)).toPoint());
    setWindowTitle(settings->value("windowTitle", "CamGui (corrupted .ini)").toString());
    coeff = settings->value("stepsInDegree", 20).toDouble();
    ui->pathLine->setText(settings->value("lastPath", "C:").toString());
    ui->pathLine->setStyleSheet("QLineEdit { background-color : yellow; }");
    ui->numBox->setValue(settings->value("lastEpitaxy", 0000).toInt());
    ui->numBox->setStyleSheet("QSpinBox { background-color : yellow; }");
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
    QObject::connect(&cc, SIGNAL(saved()), this, SLOT(saved()));
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
    QObject::connect(&cc, SIGNAL(captured(QPixmap)), this, SLOT(accuire(QPixmap)));
    QObject::connect(ui->motButton, &QPushButton::pressed, this, &MainWindow::motConnect);
    QObject::connect(ui->lButton, &QPushButton::pressed, this, &MainWindow::l);
    QObject::connect(ui->rButton, &QPushButton::pressed, this, &MainWindow::r);
    QObject::connect(ui->l10Button, &QPushButton::pressed, this, &MainWindow::l10);
    QObject::connect(ui->r10Button, &QPushButton::pressed, this, &MainWindow::r10);
    QObject::connect(ui->posSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::setPos);
    QObject::connect(ui->thetaButton, &QPushButton::pressed, this, &MainWindow::scan);
    QObject::connect(ui->pathLine, &QLineEdit::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->numBox, &QSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->seriesBox, &QSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->shotBox, &QSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->seqBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->thetaStepSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->fromSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->toSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->posSpinBox, &QDoubleSpinBox::editingFinished, this, &MainWindow::edited);
    QObject::connect(ui->pathLine, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textEdited), this, &MainWindow::editing);
    QObject::connect(ui->numBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->seriesBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->shotBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->seqBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->thetaStepSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->fromSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->toSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->posSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &MainWindow::editing);
    QObject::connect(ui->stopButton, &QPushButton::pressed, this, &MainWindow::stop);
    ui->liveLable->setMouseTracking(true);
    emit savePath(ui->pathLine->text());
    setControlsEnabled(false);
    center = settings->value("center", QPoint(0, 0)).toPoint();
    radius = settings->value("radius", 10).toInt();
    eps = settings->value("eps", 20).toReal();
    comThread->start();

}

MainWindow::~MainWindow()
{
    comThread->quit();
    comThread->requestInterruption();
    if(LOG){
        log->print("QThread dead ? : ");
    }
    saveSettings();
    if(comThread->wait()){
        if(LOG){
            log->print("True");
        }
    }else{
        if(LOG){
            log->print("False");
        }
    }
    delete comThread;
    timer->stop();
    delete timer;
    live->stop();
    delete live;
    emit disconnect();
    delete ui;
    delete settings;
    if(LOG){
        delete log;
    }
}

void MainWindow::saveSettings(){
    settings->setValue("lastPath", ui->pathLine->text());
    settings->setValue("lastEpitaxy", ui->numBox->value());
    settings->setValue("center", center);
    settings->setValue("windowSize", this->size());
    settings->setValue("windowPos", QPoint(this->x(), this->y()));
}

void MainWindow::connectionResult(bool res){
    connected = res;
    ui->seqButton->setEnabled(connected);
    ui->shotButton->setEnabled(connected);
    if(connected){
        live->start(settings->value("fpsTimer", 40).toInt());
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
        ui->pathLine->setStyleSheet("QLineEdit { background-color : white; }");
    }
}

void MainWindow::setPath(){
    emit savePath(ui->pathLine->text());
}

void MainWindow::singleShot(){
    if(DEBUG){
        log->print("shot " + now.currentDateTime().toString("mm:ss:zzz"));
    }
    emit capture(filename());
    ui->shotBox->setValue(ui->shotBox->value() + 1);
}

QString MainWindow::filename(){
    QString res = "v" + QString("%1").arg(ui->numBox->value(), 4, 10, QChar('0'));
    res += "-";
    res += QString("%1").arg(ui->seriesBox->value(), 2, 10, QChar('0'));
    QString dir = ui->pathLine->text() + "/" + res;
    if(!QDir(dir).exists()){
        QDir().mkdir(dir);
    }
    if(lastDir != dir){
        lastDir = dir;
        param = new QSettings(dir + "/" + res + ".param", QSettings::IniFormat);
        if(DEBUG){
            log->print("param created " + res);
        }
    }
    emit savePath(dir);
    res += "-";
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
        ui->numBox->setEnabled(true);
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
        ui->numBox->setEnabled(false);
        timer->start(1000*ui->seqBox->value());
    }
}

void MainWindow::liveShot(){
    if(DEBUG){
        //log->print("liveShot ");
    }
    emit capture("");
}

void MainWindow::accuire(QPixmap pixMap){
    if(DEBUG){
        //log->print("accuire ");
    }
    if(!pixMap.isNull()){
            prev[prevIndex] = time.currentMSecsSinceEpoch();
            if(prevIndex == 4){
                prevIndex = 0;
            }else{
                prevIndex++;
            }
            qreal tmp = 0;
            for(int i = 0; i < 5; i++){
                tmp += prev[i];
            }
            QPainter painter(&pixMap);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setBrush(brush);
            pen.setColor(QColor(250,0,0));
            painter.setPen(pen);
            painter.drawEllipse(center, radius, radius);
            ui->fpsLabel->setText(QString::number((int)(1000/(time.currentMSecsSinceEpoch() - tmp/5))));
            ui->liveLable->setPixmap(pixMap);
            ui->radioButton->setChecked(!ui->radioButton->isChecked());
    }
}

void MainWindow::closed(){
    opened = false;
    scanning = false;
    setControlsEnabled(false);
    if(DEBUG){
        log->print("port closed");
    }
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
        if(DEBUG){
            log->print("line finished = " + inBuf);
        }
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
    }else{
        if(DEBUG){
            log->print("not finished: " + inBuf);
        }
    }
}

void MainWindow::motConnect(){
    emit close();
    timer->stop();
    if(opened){
        opened = false;
        scanning = false;
    }else{
        if(LOG){
            log->print("connecting to " + settings->value("portName", "COM1").toString());
        }
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
        if(DEBUG){
            log->print("scan   begin");
        }
        fromBegin.start();
        newPos(ui->fromSpinBox->value());
        scanning = true;
        setControlsEnabled(false);
        ui->seqButton->setEnabled(false);
    }else{
        if(DEBUG){
            log->print("scan    make shot");
        }
        if(prevPos != pos){
            prevPos = pos;
            singleShot();
            if(ui->fromSpinBox->value() < ui->toSpinBox->value()){
                emit send(QString::number(d2s(ui->thetaStepSpinBox->value())));
            }else if(ui->fromSpinBox->value() > ui->toSpinBox->value()){
                emit send(QString::number(d2s(-ui->thetaStepSpinBox->value())));
            }
            if(DEBUG){
                log->print("scan    moved");
        }
        }
        if(ui->toSpinBox->value() == s2d(pos) ||
                ((ui->fromSpinBox->value() < ui->toSpinBox->value()) && ui->toSpinBox->value() <= s2d(pos)) ||
                ((ui->fromSpinBox->value() > ui->toSpinBox->value()) && ui->toSpinBox->value() >= s2d(pos))){
            if(DEBUG){
                log->print("scan    stopped, last shot");
            }
            singleShot();
            scanning = false;
            setControlsEnabled(true);
            ui->seqButton->setEnabled(true);
            if(DEBUG){
                log->print("scan    finished");
            }
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
    }else if(from == ui->numBox){
        ui->numBox->setStyleSheet("QSpinBox { background-color : yellow; }");
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
    }else if(from == ui->numBox){
        ui->numBox->setStyleSheet("QSpinBox { background-color : white; }");
        ui->seriesBox->setValue(0);
        ui->shotBox->setValue(0);
    }
}

void MainWindow::stop(){
    scanning = false;
    seq = false;
    emit send(QString::number(0));
    setControlsEnabled(true);
    ui->seqButton->setEnabled(true);
}

void MainWindow::saved(){
    if(LOG){
        log->print("saved");
    }
    QString name = "v" + QString("%1").arg(ui->numBox->value(), 4, 10, QChar('0'));
    name += "-";
    name += QString("%1").arg(ui->seriesBox->value(), 2, 10, QChar('0'));
    name += "-";
    name += QString("%1").arg(ui->shotBox->value(), 4, 10, QChar('0'));
    name += ".jpg";
    param->beginGroup(name);
        param->setValue("TimeStamp1", now.currentDateTime().toString("dd.MM.yyyy HH:mm:ss"));
        param->setValue("timeStamp2", fromBegin.elapsed());
        param->setValue("phi", settings->value("phiOffset", 0).toDouble());
        param->setValue("th", ui->posSpinBox->value());
        param->setValue("i0", center.x());
        param->setValue("j0", center.y());
        /* unused
        param->setValue("ISO", "");
        param->setValue("AV", "");
        param->setValue("TV", "");
        param->setValue("LBound", "");
        param->setValue("RBound", "");
        */
        param->setValue("pixelSize", settings->value("pixelSize", 0).toDouble());
        param->setValue("distance", settings->value("distance", 0).toDouble());
        param->setValue("Goniometer", "Rheed");
        param->setValue("Detector", "Panasoic");
        param->setValue("Lambda", settings->value("lambda", 0).toDouble());
        param->setValue("RotateRawImage", settings->value("rotateImage", 0).toDouble());
        param->setValue("FlipRawImage", settings->value("flipImage", 0).toInt());
        param->setValue("SampleNo", ui->numBox->value());
        param->setValue("phiStart", 0);
        param->setValue("phiEnd", 0);
        param->setValue("thStart", ui->fromSpinBox->value());
        param->setValue("thEnd", ui->toSpinBox->value());
        param->setValue("timeSequence", seq);
        param->setValue("thetaScan", scanning);
    param->endGroup();
}


void MainWindow::mouseMoveEvent(QMouseEvent * event){
    if(pow(eps, 2) >= (pow((center.x() - event->x() + ui->liveLable->x()), 2) + pow((center.y() - event->y() + ui->liveLable->y()), 2))){
        center = QPoint(event->x() - ui->liveLable->x(), event->y() - ui->liveLable->y());
    }
}
