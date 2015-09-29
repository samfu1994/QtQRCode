#include "combine.h"

combine::combine()
{
    halftone = NULL;
    qrcodeimg = NULL;
    dst = NULL;
}

void combine::feed(IplImage *h, IplImage *q, QRcode *qr){
    if(h == NULL || q == NULL || qr == NULL){
        return;
    }
    qrcode = qr;
    halftone = cvCloneImage(h);
    qrcodeimg = cvCloneImage(q);
    run();
}

/*
 * combine halftone image with qrcode.
 * Leave the center pixes unchanged, also leave none-Data
 * modules unchanged, because they are very important for
 * correctly decode qrcode.
 * The imformation of qrcode is organized as below:
 *
 * Symbol data is represented as an array contains width*width uchars.
 * Each uchar represents a module (dot). If the less significant bit of
 * the uchar is 1, the corresponding module is black. The other bits are
 * meaningless for usual applications, but here its specification is
 * described.
 *
 * MSB 76543210 LSB
 *     |||||||`- 1=black/0=white
 *     ||||||`-- data and ecc code area
 *     |||||`--- format information
 *     ||||`---- version information
 *     |||`----- timing pattern
 *     ||`------ alignment pattern
 *     |`------- finder pattern and separator
 *     `-------- non-data modules (format, timing, etc.)
 */
int combine::process(){
    if(halftone == NULL || qrcodeimg == NULL)
        return -1;
    if(halftone->width != qrcodeimg->width || \
            halftone->height != qrcodeimg->height)
        return -1;
    if(dst)
        cvReleaseImage(&dst);
    dst = cvCloneImage(halftone);
    unsigned char *pdst =
            (unsigned char *)dst->imageData;
    unsigned char *pqrc =
            (unsigned char *)qrcodeimg->imageData;
    for(int y = 0; y < dst->height; y += 1){
        for(int x = 0; x < dst->width; x += 1){
            if(((x/3) % 3 == 1 && (y/3) % 3 == 1) ||
                    (qrcode->data[qrcode->width*(y/9) + x/9] & 0x02) == 0){
                pdst[x] = pqrc[x];
            }else{
                pdst[x] = (pdst[x]>> 4) | (pdst[x] & 0xF0);
            }
        }
        pdst += dst->widthStep;
        pqrc += qrcodeimg->widthStep;
    }
    return 0;
}

IplImage * combine::getdata(){
    return dst;
}
