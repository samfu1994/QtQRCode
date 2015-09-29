#include "qrcodegen.h"

int QRCodeGen::feed(string msg){
    if(msg == "")
        return 0;
    this->msg = msg;
    run();
    return 0;
}

/*
 * generate an image of Qrcode
 */
int QRCodeGen::process(){
    if (qrcode){
        QRcode_free(qrcode);
    }
    qrcode = QRcode_encodeString(msg.data(), 0, QR_ECLEVEL_H, QR_MODE_8, 1);
    if(qrcode == NULL){
        return -1;
    }
    this->size = qrcode->width * 9;
    IplImage * img;
    CvSize imgsize = cvSize(qrcode->width * 9, qrcode->width * 9);
    img = cvCreateImage(imgsize, IPL_DEPTH_8U, 1);
    unsigned char * data = (unsigned char *)img->imageData;
    memset(data, 0xff, img->imageSize);
    int qrwith = qrcode->width * 9;
    for (int y = 0; y < qrwith; y++){
        for (int x = 0; x < qrwith; x++){
            if (qrcode->data[qrcode->width * (y / 9) + (x / 9)] & 0x01){
                data[x] = 0;
            }else{
                data[x] = 255;
            }
        }
        data += img->widthStep;
    }
    if (qrimg){
        cvReleaseImage(&qrimg);
    }
    qrimg = img;
    return 0;
}

int QRCodeGen::getSize(void){
    return size;
}

QRcode* QRCodeGen::getQrData(void){
    return qrcode;
}

IplImage* QRCodeGen::getdata(){
    return qrimg;
}

QRCodeGen::QRCodeGen(){
    qrcode = NULL;
    qrimg = NULL;
}
