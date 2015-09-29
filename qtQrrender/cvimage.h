/*
 * this class is the interface between opencv image
 * and qt Qimage class.
 * the input of this class is an opencv image and it can
 * then be drawn on a surface of qt directly
 */

#ifndef CVIMAGE_H
#define CVIMAGE_H

#include <QGraphicsPixmapItem>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <QObject>
using namespace std;

class cvImage : public QObject ,public QGraphicsPixmapItem
{
Q_OBJECT
private:
    IplImage *img;
    QImage * qimg;
    QPointF lastpos;
    void updataImgMask();
    void drawMask(QPointF p1, QPointF p2);
public:
    explicit cvImage();
    explicit cvImage(IplImage * img);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(
            QPainter *painter,
            const QStyleOptionGraphicsItem *item,
            QWidget *widget);
    IplImage * getimg(void);
    void setimg(IplImage *img);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

};

#endif // CVIMAGE_H
