#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "Core/qrrenderCore.h"
#include "Core/qrencode/qrencode.h"
#include <string>
#include "Core/qrcodegen.h"
#include "Core/combine.h"
#include "Core/halftonegen.h"
#include "Core/qrhighquality.h"
#include <stdlib.h>
#include <Core/ZBar/include/zbar.h>
#include <fstream>

using namespace std;
using namespace cv;
using namespace zbar;

int stat[1000][1000] = {0};

string generateRandString(int len){
    static char testSet[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM!@#$%^&*()_+-=~\\/,.<>:\"";
    char buf[2000];
    for(int i = 0; i < len; i++){
        buf[i] = testSet[rand()%(sizeof(testSet)-1)];
    }
    buf[len] = 0;
    return string(buf);
}

IplImage * addEdge(IplImage* img){
    int edgeWidr = 30;
    int edgeWidl = 34 - ((img->width) % 4);
    CvSize size;
    size.width = edgeWidr + edgeWidl + img->width;
    size.height = edgeWidr + edgeWidr +img->height;
    IplImage * res = cvCreateImage(size,img->depth,img->nChannels);
    memset(res->imageData,0xff,res->imageSize);
    for(int i = 0; i < img->height; i++){
        memcpy(res->imageData+(i+edgeWidr)*res->widthStep+edgeWidl,
               img->imageData+i*img->widthStep,
               img->width);
    }
    size.height *= 2;
    size.width *= 2;
    IplImage * res1 =  cvCreateImage(size,img->depth,img->nChannels);
    cvResize(res,res1,CV_INTER_NN);
    return res1;
}

IplImage* rotateImage(IplImage* img, int degree)
{
    double angle = degree  * CV_PI / 180.;
    double a = sin(angle), b = cos(angle);
    int width=img->width, height=img->height;
    int width_rotate= int(height * fabs(a) + width * fabs(b));
    int height_rotate=int(width * fabs(a) + height * fabs(b));
    IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), img->depth, img->nChannels);
    cvZero(img_rotate);
    int tempLength = sqrt((double)width * width + (double)height *height) + 10;
    int tempX = (tempLength + 1) / 2 - width / 2;
    int tempY = (tempLength + 1) / 2 - height / 2;
    IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), img->depth, img->nChannels);
    cvZero(temp);
    cvSetImageROI(temp, cvRect(tempX, tempY, width, height));
    cvCopy(img, temp, NULL);
    cvResetImageROI(temp);
    float m[6];
    int w = temp->width;
    int h = temp->height;
    m[0] = b;
    m[1] = a;
    m[3] = -m[1];
    m[4] = m[0];
    m[2] = w * 0.5f;
    m[5] = h * 0.5f;
    CvMat M = cvMat(2, 3, CV_32F, m);
    cvGetQuadrangleSubPix(temp, img_rotate, &M);
    cvReleaseImage(&temp);
    return img_rotate;
}

bool testCode(string msg, IplImage * qrimg){
    ImageScanner scanner;
    Image image(qrimg->widthStep, qrimg->height, "Y800", qrimg->imageData, qrimg->imageSize);
    int n = scanner.scan(image);
//    cout << "N = " << n << " " << qrimg->width << " " << qrimg->widthStep<< endl;
    if(n == 0){
//        cout << "no" << endl;
        image.set_data(NULL, 0);
        return false;
    }
    Image::SymbolIterator symbol = image.symbol_begin();
    string decoded = string(symbol->get_data());
    if(decoded == msg){
//        cout << "yes" << endl;
        image.set_data(NULL, 0);
        return true;
    }else{
//        cout << "no" << endl;
        image.set_data(NULL, 0);
        return false;
    }
    image.set_data(NULL, 0);

}

int decodeTest(){
    int num;
    ifstream beginnum("a.txt");
    beginnum >> num;
    beginnum.close();
    ofstream nextnum("a.txt");
    nextnum << num+1;
    nextnum.close();
    int devisor = 6;
    int beginNum = num;
    char windname[2];
    char name[100];
    windname[0] = num+'0';
    windname[1] = 0;
    cout << beginNum << endl;
    string testPic[] = {"test/aerial.bmp",
                        "test/airfield.bmp",
                        "test/airfield2.bmp",
                        "test/airplaneU2.bmp",
                        "test/baboon.bmp",
                        "test/barbara.bmp",
                        "test/boats.bmp",
                        "test/BoatsColor.bmp",
                        "test/bridge.bmp",
                        "test/cablecar.bmp",
                        "test/cornfield.bmp",
                        "test/dollar.bmp",
                        "test/finger.bmp",
                        "test/fingerprint.bmp",
                        "test/flower.bmp",
                        "test/flowers.bmp",
                        "test/fruits.bmp",
                        "test/girl.bmp",
                        "test/goldhill.bmp",
                        "test/lenna.bmp",
                        "test/man.bmp",
                        "test/monarch.bmp",
                        "test/pens.bmp",
                        "test/pepper.bmp",
                        "test/sailboat.bmp",
                        "test/soccer.bmp",
                        "test/testpat.bmp",
                        "test/tiffany.bmp",
                        "test/yacht.bmp"
                       };
    IplImage * psimg[10];
    int testSetNum = sizeof(testPic) / 4;
    int testNum = 5;
    int minLen = 15;
    int maxLen = 250;
    int step = 10;
    string msg;
    int lentype = 0;
    sprintf(name,"%d_log.txt",beginNum);
    ofstream logout(name);
    sprintf(name,"%d.txt",beginNum);
    ofstream fout(name);
    for(int l = minLen; l < maxLen; l+= step){
        for(int j = 0; j < testNum; j++){
            cout << l << " " << j << endl;
            msg = generateRandString(l);
            logout<<"MSG "<< l << " :" << msg << endl;
            for(int i = beginNum; i < testSetNum; i+=devisor){
                logout << "IMG NUM " << i << endl;
                qrHighQuality qrh;
                // test Code
                qrh.feed(testPic[i],msg);
                if(qrh.getdata() == NULL){
                    cout << "ERROR" << endl << endl << endl;
                    continue;
                }
                IplImage * qrimg = addEdge(qrh.getdata());
                psimg[0] = qrimg;
                psimg[1] = cvCloneImage(qrimg);
                psimg[2] = cvCloneImage(qrimg);
                cvSmooth(qrimg,psimg[1],CV_GAUSSIAN,3);
                cvSmooth(qrimg,psimg[2],CV_GAUSSIAN,5);
                psimg[3] = rotateImage(qrimg,5);
                psimg[4] = rotateImage(qrimg,-5);
                psimg[5] = rotateImage(qrimg,15);
                psimg[6] = rotateImage(qrimg,-15);

                for(int ii = 0; ii < 7; ii++){
                    logout << "test num: " << ii;
                    if(testCode(msg,psimg[ii])){
                        stat[lentype][ii]++;
                        logout << " Y" << endl;
                    }else{
                        logout << " N" << endl;
                    }
                }
                cvShowImage(windname,psimg[0]);
                cvWaitKey(1);
                for(int ii = 0; ii < 7; ii++){
                    cvReleaseImage(psimg+ii);
                }
            }
        }
        for(int jj = 0;jj < 7; jj++){
            fout << stat[lentype][jj] << " ";
        }
        fout << endl;
        lentype ++;
    }
    return 0;
}
