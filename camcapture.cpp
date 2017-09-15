#include "camcapture.h"

CamCapture::CamCapture(QObject *parent) : QObject(parent){
    devCount = devMgr.deviceCount();
    for(int i = 0; i < devCount; i++){
        devNames.append(QString::fromStdString(devMgr.getDevice(i)->product.read()));
    }
    setN(0); //use as default
}

void CamCapture::setN(int n){
    if(n < devCount && n >= 0){
        if(currN != n){
            currN = n;
            connect();
        }
    }else{
        QString ex = "Wrong devNumber! got: " + QString::number(n) + ", expected from 0 to " + QString::number(devCount);
        qDebug() << ex << endl;
    }
}

void CamCapture::connect(){
    if(connected){
        curr->close();
        qDebug() << "disconnected";
        connected = false;
    }
    curr = devMgr.getDevice(currN);
    try
    {
        curr->open();
        fi = new FunctionInterface(curr); //delete while destruction
        connected = true;
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        qDebug() <<  "An error occurred while opening the device(error code: " << e.getErrorCode() << ")." << endl;
        connected = false;
    }
    emit connectionResult(connected);
}

void CamCapture::reqDevList(){
    devCount = devMgr.deviceCount();
    for(int i = 0; i < devCount; i++){
        devNames.append(QString::fromStdString(devMgr.getDevice(i)->product.read()));
    }
    emit sendDevList(devNames);
}

void CamCapture::disconnect(){
    if(connected){
        curr->close();
        connected = false;
        emit connectionResult(connected);
    }
}


void CamCapture::capture(QString filename){
    if(!connected){
        emit captured(QPixmap());
    }else{
        fi->imageRequestSingle();
        int requestNr = fi->imageRequestWaitFor( iMaxWaitTime_ms );
        if( !fi->isRequestNrValid( requestNr ) )
        {
            qDebug() << "imageRequestWaitFor failed (" << requestNr << ", " << QString::fromStdString(ImpactAcquireException::getErrorCodeAsString( requestNr )) << ")"
                 << ", timeout value too small?" << endl;
            emit captured(QPixmap());
        }else{
            const Request* pRequest = fi->getRequest( requestNr );
            if( !pRequest->isOK() )
            {
                qDebug() << "Error: " << QString::fromStdString(pRequest->requestResult.readS()) << endl;
                emit captured(QPixmap());
            }else{
                x = pRequest->imageWidth.read();
                y = pRequest->imageHeight.read();
                pitch = pRequest->imageLinePitch.read();
                bpp = pRequest->imagePixelPitch.read() * 8;
                QByteArray data;

                    BITMAPINFOHEADER    bih;
                    BITMAPFILEHEADER    bfh;
                    quint16             linelen = ( ( x * bpp + 31 ) / 32 * 4 );
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
                    bih.biBitCount      = static_cast<quint16>( bpp );
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
                    char* begin = reinterpret_cast<char*>( pRequest->imageData.read() );
                    for( YPos = YStart + y - 1; YPos >= YStart; YPos-- ){
                        data.append(&begin[YPos * pitch], linelen);
                    }
                fi->imageRequestUnlock( requestNr );
                pixMap.loadFromData(data, "BMP");
                if(filename.length() != 0){ //save to file, if filename was passed to this function
                    pixMap.save(path + filename + ".jpg", "JPG", 100);
                    emit saved();
                }
                emit captured(pixMap);
            }
        }
    }
}

void CamCapture::setPath(QString data){
    path = data + "/";
}
