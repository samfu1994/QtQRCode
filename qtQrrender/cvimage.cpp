#include "cvimage.h"
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>

cvImage::cvImage(){
    this->img = NULL;
    this->qimg = NULL;
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);
}
cvImage::cvImage(IplImage * img){
    this->img = img;
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptHoverEvents(true);
}

QRectF cvImage::boundingRect() const{
    if(img == NULL){
        return QRectF(0,0,0,0);
    }else{
        return QRectF(0, 0, img->width, img->height);
    }
}
QPainterPath cvImage::shape() const{
    QPainterPath path;
    if(img == NULL){
        path.addRect(0, 0, 0, 0);
    }else{
        path.addRect(0, 0, img->width, img->height);
    }
    return path;
}
void cvImage::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *item,
                    QWidget *widget){
    QGraphicsPixmapItem::paint(painter,item,widget);
}

IplImage * cvImage::getimg(void){
    return img;
}

void cvImage::setimg(IplImage *img){
    if(img == NULL)
           return;
    if(this->img != NULL)
        cvReleaseImage(&this->img);
    if(this->qimg != NULL)
        delete qimg;
    this->img = cvCreateImage(cvSize(img->width,img->height),img->depth,3);
    cvConvertImage(img,this->img);
    cvCvtColor(this->img,this->img,CV_BGR2RGB);
    qimg = new QImage(
                (unsigned char *)this->img->imageData,
                img->width,
                img->height,
                QImage::Format_RGB888);
    this->setPixmap(QPixmap::fromImage(*qimg));
}

void cvImage::mousePressEvent(QGraphicsSceneMouseEvent *event){
    lastpos = event->pos();
    update();
}

void cvImage::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    lastpos = event->pos();
    update();
}


void cvImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *){
//    QGraphicsItem::mouseReleaseEvent(event);
    update();
}


