#ifndef QRHIGHQUALITY_H
#define QRHIGHQUALITY_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "qrrenderCore.h"
#include "qrencode/qrencode.h"
#include <string>
#include "qrcodegen.h"
#include "combine.h"
#include "halftonegen.h"

using namespace std;
using namespace cv;

class qrHighQuality : public render
{
#if __USING_QT__
Q_OBJECT
#endif

private:
    IplImage *img;
    IplImage *res;
    QRCodeGen qrcodegen;
    halftoneGen halftonegen;
    combine cmb;


    int process();
    int iter(IplImage * img, IplImage * ref);
    void releaseCenter(IplImage *t, IplImage* ref, int num);

    const int windowSize;
    const int sigma;
    const int fillEdgeColor;
    Mat gauss;

    IplImage* createTempImg(const IplImage*);

public:
    qrHighQuality();
    IplImage* getdata(void);
    IplImage* getsrc(void);
    int feed(string imgPath, string encodeMsg);
#if __USING_QT__
signals:
    void srcImgReady();
#endif
};

#endif // QRHIGHQUALITY_H
