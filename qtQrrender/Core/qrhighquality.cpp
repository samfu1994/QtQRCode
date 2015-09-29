#include "qrhighquality.h"

/*
 * private function declearation
 */

int qrhq_mask[5000][5000];
typedef struct _node{
    int x;
    int y;
    int v;
} ss;

#define ITER_TIME 20
#define AFTER_ITER_TIME 3
#define ALLOW_PERCENT 10
/*
 *  apply filter kernel to img at position (x,y)
 * return the value of pixel (x,y) after process
 */
int getval(int x, int y, const IplImage* img, const Mat & kernel);

void swap_node(ss *a, ss *b){
    int tmp_x, tmp_y, tmp_v;
    tmp_x = a -> x; tmp_y = a -> y; tmp_v = a -> v;
    a -> x = b -> x; a -> y = b -> y; a -> v = b -> v;
    b -> x= tmp_x; b -> y = tmp_y; b -> v = tmp_v;
    return;
}

int divide(int low, int high, ss ** array){
    int key = array[low] -> v, key_x = array[low] -> x, key_y = array[low] -> y;
    int i = low, j = high;
    while(i < j){
        while(i < j && array[j] -> v >= key) j--;
        if(i < j){
            swap_node(array[j], array[i]);
            i++;
        }
        while(i < j && array[i] -> v <= key) i++;
        if(i < j){
            swap_node(array[j], array[i]);
            j--;
        }
    }
    array[i] -> v = key;
    array[i] -> x = key_x;
    array[i] -> y = key_y;
    return i;
}

void quickSort(int low, int high, ss ** array){
    if(low >= high) return;
    int mid = divide(low, high, array);
    quickSort(low, mid - 1,array);
    quickSort(mid + 1, high, array);
    return;
}

qrHighQuality::qrHighQuality():
    windowSize(1),
    sigma(1),
    fillEdgeColor(255),
    gauss(windowSize*2+1, windowSize*2+1,CV_32FC1 )
{
    img = NULL;
    res = NULL;
    // width = 0;
    Mat m = getGaussianKernel(windowSize*2+1,sigma,CV_32FC1);
    for(int i = 0; i < windowSize*2+1; i++){
        for(int j = 0; j < windowSize*2+1; j++){
            gauss.at<float>(i,j) = m.at<float>(i)* m.at<float>(j);
        }
    }
}

IplImage* qrHighQuality::getdata(void){
    return res;
}

IplImage* qrHighQuality::getsrc(void){
    return img;
}

inline int getval(int x, int y, const IplImage* img, const Mat & kernel){
    int len = kernel.size().width / 2;
    int l = kernel.size().width;
    float sum = 0;
    unsigned char * t = (unsigned char *)img->imageData + (x-len) + (y-len) * img->widthStep;
    for(int y = 0;y < l; y++){
        for(int x = 0; x < l; x++){
            sum += kernel.at<float>(x,y) * t[x];
        }
        t += img->widthStep;
    }
    return sum;
}

/*
 * first calculate how many centers can be changed, named m;
 * then change them (m/10) by (m/10), each time change m/10 centers, though some
 * unpleasant things may happen during one time.
 * finally, after 10 time change, we will do some iteration to get a better result
 * which is named AFTER_ITER TIME
 */
int qrHighQuality::process(){
    // create temp image
    IplImage *tt,*gsrc;
    tt = cmb.getdata();
    gsrc = img;
    IplImage *t = createTempImg(tt);
    int prevNum = 0;
    int changeNum = 0;
    for(int i = 0; i < ITER_TIME; i++){
        changeNum = iter(t,gsrc);
        if(std::abs(prevNum - changeNum) < 20)
            break;
        prevNum = changeNum;
    }

    this->res = t;
    cvWaitKey(1);
    return 0;
}

IplImage* qrHighQuality::createTempImg(const IplImage *img){
    CvSize size = cvSize(img->width + windowSize*2,
                         img->height + windowSize*2);
    // create image with edge
    IplImage * processimg =
            cvCreateImage(size,
                          img->depth,
                          img->nChannels);
    char * tmpdata = processimg->imageData;
    char * srcdata = img->imageData;
    // fill white color for egde
    for(int i = 0; i < windowSize; i++){
        memset(tmpdata,fillEdgeColor,processimg->widthStep);
        tmpdata += processimg->widthStep;
    }
    for(int i = 0; i < img->height ; i++){
        memset(tmpdata,fillEdgeColor,windowSize);
        memcpy(tmpdata+windowSize,srcdata,img->width);
        memset(tmpdata+windowSize+img->width,fillEdgeColor,windowSize);
        srcdata+= img->widthStep;
        tmpdata+= processimg->widthStep;
    }
    for(int i = 0 ; i < windowSize; i++){
        memset(tmpdata,fillEdgeColor,processimg->widthStep );
        tmpdata+= processimg->widthStep;
    }
    processimg->width = img->width;
    processimg->height = img->height;
    processimg->imageData += windowSize + windowSize * processimg->widthStep;
    return processimg;
}

/*
 *   Calculate dErr(x,y),by toggling or swaping
 * e = (r-a)^2
 * derr = (r-a)^2 - (r0-a)^2
 *      = r^2 + a^2 -2*r*a - a^2 - r0^2 + 2*r0*a
 *      = (r-r0)(r+r0) - 2*a(r-r0)
 *      = (r-r0)(r+r0-2*a)
 *      = dr*(dr+2*r0-2*a)
 *      = dr*(dr+2*(r0-a))
 *      = dr*(dr+2*e0)
 *
 * dr for center and for pix around is is calculated blowe
 * e0 is calculated only once for each iteration, and updated if
 * a toggle or swap is finally token.
 *
 * Notice that dr for center pixel is already calculated and stored
 * in array dr, then for every pix around xx,yy, dr is calculated as
 * drcent + dr(toggle xx,yy);
 * drcent's index has a offset of windowsize, Other element is 0, thus
 * convenient for using, the edge effect is considered in it.
 *
 * This part tries to toggle pixel suround the center pixel and
 * calculate the err difference of swap.
 * Notice that swap two different pixel means toggling both pixel.
 * That is the idea of how to calculate this part
 */

int qrHighQuality::iter(IplImage * t, IplImage * ref){
    static int e0[6000][6000] = {0};
    int derr[windowSize*2+1][windowSize*2+1];
    int drcent[windowSize*4+1][windowSize*4+1];
    int drpix[windowSize*4+1][windowSize*4+1];
    memset(drcent,0,sizeof(drcent));
    int changeCount = 0;
    int toggleCount = 0;
    unsigned char * pref= (unsigned char *)ref->imageData;
    unsigned char * pimg= (unsigned char *)t->imageData;
    // Calculate e0, see blow
    for(int j = 0; j < img->height; j++){
        for(int i = 0; i < img->width; i++){
            e0[j+windowSize][i+windowSize] = getval(i,j,t,gauss) - pref[i];
        }
        pref += ref->widthStep;
    }
    pref= (unsigned char *)ref->imageData;
    // try apply change and swap
    for(int y = 0; y < img->height-2*windowSize; y++){
        for(int x = 0; x < img->width-2*windowSize; x++){
            if(qrhq_mask[y+windowSize][x+windowSize])
                continue;
            memset(drcent,0,sizeof(drcent));
            /*
             * calculate dR(x,y) for center, center is toaggled anyway
             */
            int center_value_change = 255-2*pimg[x + windowSize + windowSize * t -> widthStep];
            for(int yy = 0; yy <= 2*windowSize; yy++){
                for(int xx = 0; xx <= 2*windowSize; xx++){
                    drcent[yy+windowSize][xx+windowSize] =
                            gauss.at<float>(2*windowSize-xx,2*windowSize-yy) *
                            center_value_change;
                }
            }
            /*
             * calculate derr if swap two points
             */
            for(int yy = 0; yy <= 2*windowSize; yy++){
                for(int xx = 0; xx <= 2*windowSize; xx++){
                    int flag = 1;
                    derr[yy][xx] = 0;
                    // copy data from dr center
                    memcpy(drpix,drcent,sizeof(drpix));
                    //just change the center itself, which means toggle
                    if(yy == windowSize && xx == windowSize){
                        flag = 0 ;
                    }
                    // calculate dr for swap pix xx,yy and center
                    int pix_value_change = 255-2*pimg[xx+x+t->widthStep*(yy)];
                    if(flag){
                        for(int yyy = -windowSize; yyy <= windowSize; yyy++){
                            for(int xxx = -windowSize; xxx <= windowSize; xxx++){
                                drpix[yy+yyy+windowSize][xx+xxx+windowSize] +=
                                        gauss.at<float>(windowSize-xxx,windowSize-yyy)*
                                        pix_value_change;
                            }
                        }
                    }
                    // calculate derr for changed area
                    if(pimg[x + xx+ yy * t-> widthStep]
                            == pimg[x + windowSize + windowSize * t -> widthStep]
                            &&(!(yy == windowSize && xx == windowSize))){
                        derr[yy][xx] = 0;
                    }else if ( (((x+ xx)/3)%3 == 1) && (((y + yy)/3)%3 == 1) ){
                        derr[yy][xx] = 0;
                    }else{
                        for(int y1 = 0; y1 <= 4*windowSize; y1++){
                            for(int x1 = 0; x1 <= 4*windowSize; x1++){
                                derr[yy][xx]+=
                                        drpix[y1][x1]*
                                        (drpix[y1][x1]+
                                         2*e0[y1+y][x1+x]);
                            }
                        }
                    }
                }
            }
            // pick the smallest one and then update e0.
            int minval = 2000000;
            int dx = -1, dy = -1;
            for(int yy = 0; yy <= 2*windowSize; yy++){
                for(int xx = 0; xx <= 2*windowSize; xx++){
                    if(qrhq_mask[y+yy][x+xx])
                        continue;
                    if(derr[yy][xx] < minval){
                        minval = derr[yy][xx];
                        dy = yy;
                        dx = xx;
                    }
                }
            }
            if(dx < 0 || minval >= 0)
                continue;
            if(!(dy == windowSize && dx == windowSize) ){//swap
                pimg[x + windowSize + windowSize * t -> widthStep] = ~pimg[x + windowSize + windowSize * t -> widthStep];
                pimg[x + dx + t -> widthStep * dy] = ~pimg[x + dx + t -> widthStep * dy ];
                int cur;
                int xmin,xmax,ymin,ymax;
                xmin = max(0,x - windowSize);
                xmax = min(t->width, x+3*windowSize+1);
                ymin = max(0,y - windowSize);
                ymax = min(t->height,y+3*windowSize +1);
                for(int yy = ymin; yy <= ymax; yy++){
                    for(int xx = xmin; xx <= xmax; xx++){
                        cur = (yy) * ref -> widthStep + xx;
                        e0[yy + windowSize][xx+ windowSize] = getval(xx, yy, t, gauss) - pref[cur];
                    }
                }
                changeCount++;
            }
            else if(derr[windowSize][windowSize] < 0){//toggle
                pimg[x + windowSize + windowSize * t -> widthStep] = ~pimg[x + windowSize + windowSize * t -> widthStep];
                int cur;
                for(int yy = y; yy <= y + windowSize * 2; yy++){
                    for(int xx = x; xx <= x + windowSize * 2; xx++){
                        cur = yy * ref -> widthStep + xx;
                        e0[yy + windowSize][xx + windowSize] = getval(xx, yy, t, gauss) - pref[cur];
                    }
                }
                toggleCount++;
            }
            else{}//it's best for now
        }
        pimg +=t->widthStep;
    }
    return changeCount;
}

int qrHighQuality::feed(string imgPath, string encodeMsg){
	if(imgPath == "" || encodeMsg == "")
		return -1;
    IplImage * timg = cvLoadImage(imgPath.data());
    if(timg == NULL){
        return -1;
    }
    qrcodegen.feed(encodeMsg);
    if(qrcodegen.getSize() <= 0)
        return -1;
    halftonegen.feed(timg,qrcodegen.getSize());
    cmb.feed(halftonegen.getdata(),qrcodegen.getdata(),qrcodegen.getQrData());
    IplImage * tt = cmb.getdata();
    IplImage* tmpimg = cvCreateImage(cvSize(timg->width, timg->height), IPL_DEPTH_8U, 1);
    IplImage* gsrc = cvCreateImage(cvSize(tt->width, tt->height), IPL_DEPTH_8U, 1);
    cvConvertImage(timg, tmpimg);//灰度
    cvResize(tmpimg,gsrc,CV_INTER_LINEAR);
    cvReleaseImage(&tmpimg);
    cvReleaseImage(&timg);
    img = gsrc;

    // calculate qrhq_mask bits;
    int qrwid = qrcodegen.getSize();
    if(qrcodegen.getQrData() == NULL){
        return -1;
    }
    unsigned char * qrdata = qrcodegen.getQrData()->data;
    memset(qrhq_mask,0,sizeof(qrhq_mask));
    for(int y = 0; y < qrwid;y++){
        for(int x = 0; x < qrwid; x++){
            if(((qrdata[x/9 + (y/9)*qrwid/9] & 0x02) == 0)){
                qrhq_mask[y][x] |= 1;
            }
            if(x % 9 == 4 && y % 9 == 4)
                qrhq_mask[y][x] |= 1;
            if((x/3)%3 == 1 && (y/3)%3 == 1){
                qrhq_mask[y][x] |= 0x02;
            }
        }
    }

    run();
    return 0;
}

void swap_node(ss *a, ss *b);

int divide(int low, int high, ss ** array);

void quickSort(int low, int high, ss ** array);

void qrHighQuality::releaseCenter(IplImage *t, IplImage* ref, int num){
    int m_count = 0;
    static int e0[700][700] = {0};
    int min_size = t -> height * t->width / 9;
    int change_center;
    ss ** minArray;
    minArray = new ss *[min_size];
    for(int i = 0;i < min_size; i++)
        minArray[i] = new ss;
    int derr;
    int drcent[windowSize*4+1][windowSize*4+1];
    memset(drcent,0,sizeof(drcent));
    unsigned char * pref= (unsigned char *)ref->imageData;
    unsigned char * pimg= (unsigned char *)t->imageData;
    // Calculate e0, see blow
    for(int j = 0; j < img->height; j++){
        for(int i = 0; i < img->width; i++){
            e0[j+windowSize][i+windowSize] = getval(i,j,t,gauss) - pref[i];
        }
        pref += ref->widthStep;
    }
    pref= (unsigned char *)ref->imageData;
    // try apply change and swap
    int final_x, final_y;
    for(int y = 0; y < img->height-2*windowSize; y += 3){
        for(int x = 0; x < img->width-2*windowSize; x += 3){
            memset(drcent,0,sizeof(drcent));
            /*
             * calculate dR(x,y) for center, center is toaggled anyway
             */
            if(qrhq_mask[y+windowSize][x+windowSize] == 0)
                continue;
            if(qrhq_mask[y+windowSize][x+windowSize] & 0x01){
                continue;
            }
            int center_value_change = 255-2*pimg[x + windowSize + windowSize * t -> widthStep];
            for(int yy = 0; yy <= 2*windowSize; yy++){
                for(int xx = 0; xx <= 2*windowSize; xx++){
                    drcent[yy+windowSize][xx+windowSize] =
                            gauss.at<float>(2*windowSize-xx,2*windowSize-yy) *
                            center_value_change;
                }
            }
            derr = 0;
            for(int y1 = 0; y1 <= 4*windowSize; y1++){
                for(int x1 = 0; x1 <= 4*windowSize; x1++){
                    derr +=  drcent[y1][x1]*
                            (drcent[y1][x1]+
                             2*e0[y1+y][x1+x]);
                }
            }
            if(derr < 0 && (x != 507 && x != 0)){
                minArray[m_count] -> v = derr;
                minArray[m_count] -> y = y;
                minArray[m_count] -> x = x;
                m_count++;
            }
        }
        pimg +=t->widthStep;
    }
    quickSort(0, m_count - 1, minArray);
    pimg= (unsigned char *)t->imageData;
    num = min(num,m_count);
    for(int i = 0; i < num; i++){
        final_x = minArray[i] -> x;
        final_y = minArray[i] -> y;
        change_center++;
        pimg[final_x + windowSize + (final_y + windowSize) * t -> widthStep] = ~pimg[final_x + windowSize + (final_y +windowSize) * t -> widthStep];
    }
    for(int i = 0;i < min_size; i++)
        delete minArray;
    delete [] minArray;
    return;
}
