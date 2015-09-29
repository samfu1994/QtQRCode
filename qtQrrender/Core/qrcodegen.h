#ifndef QRCODEGEN_H
#define QRCODEGEN_H

#include "qrrenderCore.h"
#include "qrencode/qrencode.h"
#include "opencv/highgui.h"
#include "opencv/cv.h"
using namespace std;
using namespace cv;

class QRCodeGen : public render{
#if __USING_QT__
Q_OBJECT
#endif
private:
//    int edgeWidthPix;
    string msg;
    IplImage *qrimg;
    QRcode *qrcode;
    int size;
    int process();
public:
    QRCodeGen();
    int feed(string);
    IplImage* getdata(void);
    QRcode* getQrData(void);
    int getSize(void);
};

#endif // QRCODEGEN_H
