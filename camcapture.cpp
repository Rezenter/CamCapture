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
        //capture();
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
    x = 0;
    y = 0;
    if(!connected){
        emit captured(0, 0, NULL, 0, 0);
    }else{
        fi->imageRequestSingle();
        //manuallyStartAcquisitionIfNeeded( curr, *fi );
        // wait for results from the default capture queue.
        int requestNr = fi->imageRequestWaitFor( iMaxWaitTime_ms );
        //manuallyStopAcquisitionIfNeeded( curr, *fi );
        if( !fi->isRequestNrValid( requestNr ) )
        {
            // If the error code is -2119(DEV_WAIT_FOR_REQUEST_FAILED), the documentation will provide
            // additional information under TDMR_ERROR in the interface reference
            qDebug() << "imageRequestWaitFor failed (" << requestNr << ", " << QString::fromStdString(ImpactAcquireException::getErrorCodeAsString( requestNr )) << ")"
                 << ", timeout value too small?" << endl;
            x = 0;
            y = 0;
            emit captured(0, 0, NULL, 0, 0);
        }else{
            const Request* pRequest = fi->getRequest( requestNr );
            if( !pRequest->isOK() )
            {
                qDebug() << "Error: " << QString::fromStdString(pRequest->requestResult.readS()) << endl;
                // if the application wouldn't terminate at this point this buffer HAS TO be unlocked before
                // it can be used again as currently it is under control of the user. However terminating the application
                // will free the resources anyway thus the call
                // fi.imageRequestUnlock( requestNr );
                // can be omitted here.
                emit captured(0, 0, NULL, 0, 0);
            }else{
                x = pRequest->imageWidth.read();
                y = pRequest->imageHeight.read();
                pitch = pRequest->imageLinePitch.read();
                bpp = pRequest->imagePixelPitch.read() * 8;
                copy = new char[x*y];
                for(int i = 0; i < x*y; i++){
                    copy[i] = reinterpret_cast<char*>( pRequest->imageData.read() )[i]; //copy image to dynamic array in order to release buffer
                }
                // unlock the buffer to let the driver know that you no longer need this buffer.
                fi->imageRequestUnlock( requestNr );
                if(filename.length() != 0){ //save to file, if filename was passed to this function
                    SaveBMP( filename + ".bmp", copy, x, y, pitch, bpp );
                }
                emit captured(x, y, copy, pitch, bpp);
            }
        }
    }
}

int CamCapture::SaveBMP(QString filename, const char* pData, int XSize, int YSize, int pitch, int bitsPerPixel ){
    static const quint16 PALETTE_ENTRIES = 256;
    if( pData )
    {
        FILE* pFile = fopen((path + filename).toStdString().c_str(), "wb" );
        if( pFile )
        {
            BITMAPINFOHEADER    bih;
            BITMAPFILEHEADER    bfh;
            quint16             linelen = ( ( XSize * bitsPerPixel + 31 ) / 32 * 4 );  // DWORD aligned
            int                 YPos;
            int                 YStart = 0;

            memset( &bfh, 0, sizeof( BITMAPFILEHEADER ) );
            memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
            bfh.bfType          = 0x4d42;
            bfh.bfSize          = sizeof( bih ) + sizeof( bfh ) + sizeof( RGBQUAD ) * PALETTE_ENTRIES + static_cast<LONG>( linelen ) * static_cast<LONG>( YSize );
            bfh.bfOffBits       = sizeof( bih ) + sizeof( bfh ) + sizeof( RGBQUAD ) * PALETTE_ENTRIES;
            bih.biSize          = sizeof( bih );
            bih.biWidth         = XSize;
            bih.biHeight        = YSize;
            bih.biPlanes        = 1;
            bih.biBitCount      = static_cast<quint16>( bitsPerPixel );
            bih.biSizeImage     = static_cast<quint32>( linelen ) * static_cast<quint32>( YSize );

            if( ( fwrite( &bfh, sizeof( bfh ), 1, pFile ) == 1 ) && ( fwrite( &bih, sizeof( bih ), 1, pFile ) == 1 ) )
            {
                RGBQUAD rgbQ;
                for( int i = 0; i < PALETTE_ENTRIES; i++ )
                {
                    rgbQ.rgbRed      = static_cast<quint8>( i );
                    rgbQ.rgbGreen    = static_cast<quint8>( i );
                    rgbQ.rgbBlue     = static_cast<quint8>( i );
                    rgbQ.rgbReserved = static_cast<quint8>( 0 );
                    fwrite( &rgbQ, sizeof( rgbQ ), 1, pFile );
                }
                for( YPos = YStart + YSize - 1; YPos >= YStart; YPos-- )
                {
                    if( fwrite( &pData[YPos * pitch], linelen, 1, pFile ) != 1 )
                    {
                        qDebug() << "SaveBmp: ERR_WRITE_FILE: " << filename << endl;
                    }
                }
            }
            else
            {
                qDebug() << "SaveBmp: ERR_WRITE_FILE: " << filename << endl;
            }
            fclose( pFile );
        }
        else
        {
            qDebug() << "SaveBmp: ERR_CREATE_FILE: " << filename << endl;
        }
    }
    else
    {
        qDebug() << "SaveBmp: ERR_DATA_INVALID:" << filename << endl;
    }
    return 0;
}

void CamCapture::setPath(QString data){
    path = data + "/";
}
