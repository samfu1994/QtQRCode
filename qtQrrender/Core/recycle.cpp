/*
diff matrix
0	0	0
0	0	7
3	5	1
*/

IplImage *halfToneImg(IplImage * img, int width){
	static int tmp[5000][5000];
	long long int sum = 0;
	IplImage * t, *tt;
	tt = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvConvertImage(img, tt);
	Mat res(tt, 0);
	Mat tag;
	float ratio;
	if (img->width > img->height) ratio = ((float)width) / img->width;
	else ratio = ((float)width) / img->height;
	CvSize imgsize = cvSize((int)(img->width *ratio), (int)(img->height * ratio));
	resize(res, tag, imgsize);
	//imshow("picResize", tag);
	IplImage iii = IplImage(tag);
	t = cvCreateImage(cvSize(width, width), 8, 1);

	t->width = imgsize.width;
	t->height = imgsize.height;
	t->imageData += (width - imgsize.width) / 2;
	cvConvertImage(&iii, t);
	//cvShowImage("asdad",t);

	unsigned char * data = (unsigned char *)t->imageData;

	for (int y = 0; y < t->height; y++){
		for (int x = 0; x < t->width; x++){
			tmp[x][y] = 0;
			sum += data[x];
		}
		data += t->widthStep;
	}
	sum = sum / t->width / t->height;
	data = (unsigned char *)t->imageData;
	//unsigned char * ori = (unsigned char *)tt->imageData;
	for (int y = 0; y < t->height - 1; y++){
		for (int x = 1; x < t->width - 1; x++){
			tmp[x][y] += data[x];
			if (tmp[x][y] + data[x] > sum){
				data[x] = 255;
				tmp[x][y] -= 255;
			}
			else{
				data[x] = 0;
			}
			tmp[x + 1][y] += tmp[x][y] * 7 / 16;
			tmp[x - 1][y + 1] += tmp[x][y] * 3 / 16;
			tmp[x][y + 1] += tmp[x][y] * 5 / 16;
			tmp[x + 1][y + 1] += tmp[x][y] / 16;
		}
		data += t->widthStep;
	}
	t->width = width;
	t->height = width;
	t->imageData -= (width - imgsize.width) / 2;
	return t;
}

void printQrcode(QRcode * qrcode){
	for (int i = 0; i <= qrcode->width + 1; i++){
		cout << "▇";
	}
	cout << endl;
	for (int x = 0; x < qrcode->width; x++){
		cout << "▇";
		for (int y = 0; y < qrcode->width; y++){
			if (qrcode->data[qrcode->width * x + y] % 2 == 0){
				cout << "▇";
			}
			else{
				printf("  ");
			}
		}
		cout << "▇";
		printf("\n");
	}
	for (int i = 0; i <= qrcode->width + 1; i++){
		cout << "▇";
	}
	cout << endl;
}


IplImage *qrTest(QRcode * qrcode, int width, IplImage * hal){
	//width *= 3;
	IplImage * img;
	CvSize imgsize = cvSize(EDGE_WID * 2 + qrcode->width * width, EDGE_WID * 2 + qrcode->width * width);
	img = cvCreateImage(imgsize, 8, 1);
	unsigned char * data = (unsigned char *)img->imageData;
	// fill white
	for (int y = 0; y < EDGE_WID; y++){
		for (int x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (int y = imgsize.height - EDGE_WID; y < imgsize.height; y++){
		for (int x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (int y = EDGE_WID; y < imgsize.height - EDGE_WID; y++){
		for (int x = 0; x < EDGE_WID; x++){
			data[img->widthStep * y + x] = 255;
		}
		for (int x = imgsize.width - EDGE_WID; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	int qrwith = qrcode->width * width;
	for (int y = 0; y < qrwith; y++){
		for (int x = 0; x < qrwith; x++){
			if (qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x01){
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 0;
			}
			else{
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 255;
			}
		}
	}
	//IplImage * oriQrcode = cvCloneImage(img);
	cvShowImage("qrCodeOri", img);
	int xx, yy;
	for (int y = 0; y < qrwith; y++){
		for (int x = 0; x < qrwith; x++){
			if (qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x02){
				xx = x * 3 / width;
				yy = y * 3 / width;
				if (xx % 3 == 1 && yy % 3 == 1){
					int index = random_number(qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x01);
					//cout << (qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x01) << endl;
					int temp_node[9];
					for (int i = 0; i < 9; i++) temp_node[i] = pattern_lib[index][i];
					int cur = img->widthStep * (y + EDGE_WID) + x + EDGE_WID - 4;
					for (int i = 0; i < 9; i++){
						data[cur + i] = temp_node[i];
				}
				/*
				if (!(xx % 3 == 1 && yy % 3 == 1) && (hal->imageData[hal->widthStep*yy + xx] == 0 || (unsigned char)hal->imageData[hal->widthStep*yy + xx] == 255)){
					data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] =
						hal->imageData[hal->widthStep*yy + xx];
				*/
				}
			}
		}
	}
	cvShowImage("combined", img);
	return img;
}
IplImage *qrRander(QRcode * qrcode, int width, IplImage * hal){
	//width *= 3;
	IplImage * img;
	CvSize imgsize = cvSize(EDGE_WID * 2 + qrcode->width * width, EDGE_WID * 2 + qrcode->width * width);
	img = cvCreateImage(imgsize, 8, 1);
	unsigned char * data = (unsigned char *)img->imageData;
	// fill white
	for (int y = 0; y < EDGE_WID; y++){
		for (int x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (int y = imgsize.height - EDGE_WID; y < imgsize.height; y++){
		for (int x = 0; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	for (int y = EDGE_WID; y < imgsize.height - EDGE_WID; y++){
		for (int x = 0; x < EDGE_WID; x++){
			data[img->widthStep * y + x] = 255;
		}
		for (int x = imgsize.width - EDGE_WID; x < imgsize.width; x++){
			data[img->widthStep * y + x] = 255;
		}
	}
	int qrwith = qrcode->width * width;
	for (int y = 0; y < qrwith; y++){
		for (int x = 0; x < qrwith; x++){
			if (qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x01){
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 0;
			}
			else{
				data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] = 255;
			}
		}
	}
	//IplImage * oriQrcode = cvCloneImage(img);
	cvShowImage("qrCodeOri", img);
	int xx, yy;
	for (int y = 0; y < qrwith; y++){
		for (int x = 0; x < qrwith; x++){
			if (qrcode->data[qrcode->width * (y / width) + (x / width)] & 0x02){
				xx = x * 3 / width;
				yy = y * 3 / width;
				if (!(xx % 3 == 1 && yy % 3 == 1) && (hal->imageData[hal->widthStep*yy + xx] == 0 || (unsigned char)hal->imageData[hal->widthStep*yy + xx] == 255)){
					data[img->widthStep * (y + EDGE_WID) + x + EDGE_WID] =
						hal->imageData[hal->widthStep*yy + xx];
				}
			}
		}
	}
	cvShowImage("combined", img);
	return img;
}




///////////////////////////////
// back up fot main function //
///////////////////////////////

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

// qrencode head
#include "qrencode/qrencode.h"

// opencv head
#include "opencv/highgui.h"
#include "opencv/cv.h"

// zbar haed
#include "ZBar/include/zbar.h"

#include "qrrenderCore.h"


// namespace 
using namespace std;
using namespace cv;
using namespace zbar;

// usr defined const
//const int EDGE_WID = 20; // define edge with
const int pattern_num = 512;
const int pattern_bit = 9;
// usr functions
//void printQrcode(QRcode * qrcode);
//IplImage *halfToneImg(IplImage * img, int width);
/*
*rander a img showing QRCODE
*param qrcode		: qrcode object
*param width		: pix width of each module, must be multiple of 3 for convinence
*/
//IplImage *qrRander(QRcode * qrcode, int width, IplImage * hal);
//IplImage *qrTest(QRcode * qrcode, int width, IplImage * hal);
/*
*	get the importance map using sobel method
*	param	img		: grascale image
*	retval			: importance map;
*/
void getImpmap(IplImage *src, IplImage *dst);

// private defination
void rand_array(char *);
void sort_the_center(int array[][9]);
void mult(int array[][9]);
void create_lib(int array[][9]);
int random_number(int x);
int pattern_lib[pattern_num][pattern_bit];

int main(){
	srand(unsigned(time(NULL)));
	printf("started\n");
	char * array; array = new char[20];
	rand_array(array);
	//int pattern_lib[pattern_num][pattern_bit];
	create_lib(pattern_lib);
	sort_the_center(pattern_lib);
	mult(pattern_lib);
	QRcode *qrcode;
	qrcode = QRcode_encodeString(array,0, QR_ECLEVEL_H, QR_MODE_8, 1);
	
	//qrcode = QRcode_encodeString("http://v.youku.com/v_show/id_XODMzMTU3MDA0.html?from=y1.3-idx-grid-1519-9909.86858-86853-86850.1-1", 0, QR_ECLEVEL_H, QR_MODE_8, 1);
	printf("finished\n");
	cout << qrcode->version << "\t" << qrcode->width << endl;

	IplImage * img, *halftoneimg;
	img = cvLoadImage("1.jpg", CV_LOAD_IMAGE_COLOR);
	//img = cvLoadImage("D:\a.jpg", CV_LOAD_IMAGE_COLOR);
	halftoneimg = halfToneImg(img, qrcode->width * 3);

	//IplImage *qrimg = qrRander(qrcode, 12, halftoneimg);
	IplImage *qrimg = qrTest(qrcode, 3, halftoneimg);
	cvShowImage("oripic", img);
	cvShowImage("half", halftoneimg);
	//cvShowImage("qrcode", qrimg);

	// imp map show
	Mat mGray, mSrc(img, 1);
	cvtColor(mSrc, mGray, CV_BGR2GRAY, 0);
	cout << mGray.channels() << endl;
	IplImage*cvt = new IplImage(mGray);
	IplImage* srcImpimg = cvCreateImage(mSrc.size(),IPL_DEPTH_8U,1);
	IplImage* tag = cvCreateImage(mSrc.size(), IPL_DEPTH_8U, 1);
	cvConvert(cvt, srcImpimg);
	getImpmap(srcImpimg, tag);
	cvShowImage("imp", tag);

	//int imp_height = tag->height;
	//int imp_width = tag->width;
	//unsigned char * img_data = (unsigned char *)tag->imageData;
	//for (int i = 0; i < imp_height; i++){
	//	for (int j = 0; j < imp_width; j++){
	//		cout << (int)img_data[j] << " ";
	//	}
	//	img_data += tag->widthStep;
	//	cout << endl;
	//}
	//cout << "chanels " << tag->nChannels<< endl;

	// zbar test
	cvSmooth(qrimg, qrimg);
	cvSmooth(qrimg, qrimg);
	cvSmooth(qrimg, qrimg);
	cvShowImage("zbar in", qrimg);
	ImageScanner scanner;
	Image image(qrimg->width, qrimg->height, "Y800", qrimg->imageData, qrimg->imageSize);
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int n = scanner.scan(image);
	for (Image::SymbolIterator symbol = image.symbol_begin();
		symbol != image.symbol_end();
		++symbol) {
		// do something useful with results
		cout << "decoded " << symbol->get_type_name()
			<< " symbol \"" << symbol->get_data() << '"' << endl;
	}
	image.set_data(NULL, 0);

	// class test
	QRCodeGen gen;
	gen.feed("http://v.youku.com/v_show/id_XODMzMTU3MDA0.html?from=y1.3-idx-grid-1519-9909.86858-86853-86850.1-1");
	gen.run();

	halftoneGen hgen;
	hgen.feed(img, gen.getSize());
	hgen.run();

	waitKey(0);
	destroyAllWindows();
	
	system("pause");
	return 0;
}

void getImpmap(IplImage *src, IplImage *dst){
	//IplImage *t = cvCreateImageHeader(dst.size, dst.depth, dst.channels);
	cvSmooth(src, dst);
	cvSmooth(dst, dst);
	Sobel(Mat(dst), Mat(dst), -1, 1, 1, 5);
	return;
}





void rand_array(char * array){
	srand(unsigned(time(NULL)));
	int temp_l = rand() % 10 + 10;
	int i = 0;
	for (; i < temp_l; i++)
		array[i] = rand() % ('z' - 'A') + 'A';
	array[i] = '\0';
}
void sort_the_center(int array[][9])
{
	// 0-255:0   256-511:1
	int l = 0; 
	int h = pattern_num - 1;
	int temp[9];
	while (1){
		if (array[l][4] != 1 && l != h) l++;
		if (l == h) break;
		if (array[h][4] != 0 && l != h) h--;
		if (l == h) break;
		for (int i = 0; i < 9; i++){
			temp[i] = array[l][i];
			array[l][i] = array[h][i];
			array[h][i] = temp[i];
		}
	}
	//for (int i = 0; i < 512; i++)
	//	cout << array[i][4] << endl;
}
void mult(int array[][9]){
	for (int i = 0; i < pattern_num; i++)
	for (int j = 0; j < pattern_bit; j++)
		array[i][j] = 255 - 255 * array[i][j];
}
void create_lib(int pattern_lib[][9]){
	int bi_lib[10];
	for (int i = 0; i < 10; i++)
		bi_lib[i] = pow(2, i);
	for (int i = 0; i < pattern_num; i++){
		for (int j = 0; j < pattern_bit; j++){
			pattern_lib[i][j] = (i % bi_lib[j + 1]) / bi_lib[j];
		}
	}
}
int random_number(int x){
	return (rand() % 256) + x * 255;
}
