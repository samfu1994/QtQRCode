#ifndef COMBINE_H
#define COMBINE_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "qrrenderCore.h"
#include "qrencode/qrencode.h"

#include <QImage>

using namespace std;
using namespace cv;

class combine : public render
{
Q_OBJECT

private:
    IplImage *halftone;
    IplImage *qrcodeimg;
    IplImage *dst;
    QRcode * qrcode;
public:
    combine();
    IplImage * getdata();
    void feed(IplImage *h, IplImage *q, QRcode *qr);
    int process();

};

#endif // COMBINE_H
