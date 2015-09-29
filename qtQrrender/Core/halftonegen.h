#ifndef HALFTONEGEN_H
#define HALFTONEGEN_H

#include "qrrenderCore.h"
#include <string>
#include "opencv/highgui.h"
#include "opencv/cv.h"
using namespace std;
using namespace cv;

class halftoneGen : public render {
#if __USING_QT__
Q_OBJECT
#endif
private:
    // input msg
    IplImage *img;
    IplImage *res;
    int width;
    int process();
public:
    int feed(IplImage * img, int width);
    int feed(int size);
    int feed(string path);
    IplImage* getdata(void);
    IplImage* getsrc(void);
    halftoneGen();
#if __USING_QT__
signals:
    void srcImgReady();
#endif
};

#endif // HALFTONEGEN_H
