#include "halftonegen.h"
#include <string.h>
/**
 *      Halftone IMG information define
 */
#define QRRENDER_BLACK_VAL 0
#define QRRENDER_WHITE_VAL 0xFF

/*
 * generate halftone image for any input image
 * the hight-width ratio is unchanged during resizing
 */
int halftoneGen::process(){
    static int tmp[5000][5000];
    memset(tmp,0,sizeof(tmp));
    long long int sum = 0;
    IplImage * t, *tt;
    unsigned char * data, *datasrc;

    // create temp image
    tt = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
    memset(tt->imageData, QRRENDER_WHITE_VAL,tt->imageSize);
    cvConvertImage(img, tt);
    Mat res(tt, 0);

    // calculate scale ratio and resize src image
    float ratio;
    if (img->width > img->height) ratio = ((float)width) / img->width;
    else ratio = ((float)width) / img->height;
    CvSize imgsize = cvSize((int)(img->width *ratio), (int)(img->height * ratio));
    resize(res, res, imgsize);

    // create target image
    t = cvCreateImage(cvSize(width, width), IPL_DEPTH_8U, 1);
    memset(t->imageData, QRRENDER_WHITE_VAL,t->imageSize);

    // change size for taget image, put img at the middle
    t->width = imgsize.width;
    t->height = imgsize.height;
    t->imageData += (width - imgsize.width) / 2;
    t->imageData += ((width - imgsize.height) / 2) * t->widthStep;
    data = (unsigned char *)t->imageData;
    datasrc = res.data;

    //copy data
    for (int y = 0; y < res.size().height; y++){
        for (int x = 0; x < res.size().width; x++){
            data[x] = datasrc[x];
        }
        data += t->widthStep;
        datasrc += res.size().width;
    }
    data = (unsigned char *)t->imageData;
    sum = sum / t->width / t->height;
    data = (unsigned char *)t->imageData;

    //unsigned char * ori = (unsigned char *)tt->imageData;
    for (int y = 0; y < t->height - 1; y++){
        for (int x = 1; x < t->width - 1; x++){
            tmp[x][y] += data[x];
            if (tmp[x][y] + data[x] > sum){
                data[x] = QRRENDER_WHITE_VAL;
                tmp[x][y] -= 255;
            }
            else{
                data[x] = QRRENDER_BLACK_VAL;
            }
            tmp[x + 1][y] += tmp[x][y] * 7 / 16;
            tmp[x - 1][y + 1] += tmp[x][y] * 3 / 16;
            tmp[x][y + 1] += tmp[x][y] * 5 / 16;
            tmp[x + 1][y + 1] += tmp[x][y] / 16;
        }
        data += t->widthStep;
    }
    // fill edges
    data = (unsigned char *)t->imageData;
    for (int y = 0; y < t->height - 1; y++){
        data[0] = data[1];
        data[t->width - 1] = data[t->width - 2];
        data += t->widthStep;
    }
    for(int x = 0; x < t->width;x++){
        data[x] = data[x-t->widthStep];
    }
    t->width = width;
    t->height = width;
    t->imageData -= (width - imgsize.width) / 2;
    t->imageData -= ((width - imgsize.height) / 2) * t->widthStep;
    cvReleaseImage(&tt);
    if (this->res){
        cvReleaseImage(&(this->res));
    }
    this->res = t;
    return 0;
}

int halftoneGen::feed(int size){
    this->width = size;
    if(this->img != NULL)
        run();
    return 0;
}

int halftoneGen::feed(string path){
    if(path == "")
        return -1;
    if(this->img){
        cvReleaseImage(&this->img);
    }
    img = cvLoadImage(path.data());
    emit srcImgReady();
    if(this->width && this->width % 3 == 0){
        run();
    }
    return 0;
}

int halftoneGen::feed(IplImage * img,int width){
    if(img == NULL)
        return 0;
    this->img = img;
    this->width = width;
    emit srcImgReady();
    run();
    return 0;
}

IplImage* halftoneGen::getdata(void){
    return res;
}

IplImage* halftoneGen::getsrc(){
    return img;
}

halftoneGen::halftoneGen(){
    this->img = NULL;
    this->res = NULL;
    width = 0;
}
