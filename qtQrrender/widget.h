#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSplitter>
#include <QBoxLayout>
#include <QGraphicsScene>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include "cvimage.h"
#include "Core/qrhighquality.h"
#include "view.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QGraphicsScene *srcScene;
    QGraphicsScene *dstScene;

    View *srcView;
    View *dstView;

    cvImage *srcImg;
    cvImage *dstImg;

    QPlainTextEdit *intext;
    QPushButton *btnStart;

    QSplitter *htextSplitter;
    QSplitter *h1Splitter;
    QSplitter *vSplitter;

    qrHighQuality *qrh;
    std::string imgpath;
    std::string encodemsg;

private:
    void setuplayout(void);

private slots:
    // slot for handling QR code process
    void openFile();
    void saveFile();
    void startProcess();
    void processFinished();
    void exitsw();
    void docWindow();
    void aboutWindow();
};

#endif // WIDGET_H
