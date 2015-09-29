#include "widget.h"
#include "view.h"

#include <QPlainTextEdit>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>

using namespace cv;

std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    std::string result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

Widget::Widget(QWidget *parent) : QWidget(parent)/*,ui(new Ui::Widget)*/
{
    qrh = new qrHighQuality;
    connect(qrh,SIGNAL(processfinish()),this,SLOT(processFinished()) );
    setuplayout();
}


void Widget::setuplayout(void){
    // create variables
    srcScene = new QGraphicsScene;
    dstScene = new QGraphicsScene;
    srcImg = new cvImage;
    dstImg = new cvImage;
    intext = new QPlainTextEdit(this);
    vSplitter = new QSplitter;
    htextSplitter = new QSplitter;
    h1Splitter = new QSplitter;
    btnStart = new QPushButton("start");

    QMenuBar* fileMenu = new QMenuBar;
    // add menus
    QMenu*file = fileMenu->addMenu("&File");
    QAction* actOpen = file->addAction("Open...");
    QAction* actSave = file->addAction("Save...");
    file->addSeparator();
    QAction* actExit = file->addAction("Exit");
    connect(actOpen,SIGNAL(triggered()),this,SLOT(openFile()) );
    connect(actSave,SIGNAL(triggered()),this,SLOT(saveFile()) );
    connect(actExit,SIGNAL(triggered()),this,SLOT(exitsw()) );
    QMenu* help = fileMenu->addMenu("&Help");
    QAction* documentation = help->addAction("Documentation");
    QAction* aboutx = help->addAction("About This Software");
    connect(documentation,SIGNAL(triggered()),this,SLOT(docWindow()) );
    connect(aboutx,SIGNAL(triggered()),this,SLOT(aboutWindow()) );
    fileMenu->setFixedHeight(30);

    // connect scene to cvIMG class
    srcScene->addItem(srcImg);
    dstScene->addItem(dstImg);

    // setup text input dialog
    intext->setFixedHeight(30);

    // add text in out dialog
    htextSplitter->addWidget(intext);
    htextSplitter->addWidget(btnStart);
    connect(btnStart,SIGNAL(clicked()),this,SLOT(startProcess()) );

    // add widges in vertical spliter;
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->addWidget(fileMenu);
    vSplitter->addWidget(htextSplitter);
    vSplitter->addWidget(h1Splitter);

    // add two new view to edit view;
    srcView = new View("Source Img");
    srcView->view()->setScene(srcScene);
    h1Splitter->addWidget(srcView);

    dstView = new View("Dastenation Image");
    dstView->view()->setScene(dstScene);
    h1Splitter->addWidget(dstView);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(vSplitter);
    setLayout(layout);
}

/*
 * ---------------------------------------------------------------
 * public slots
 * ---------------------------------------------------------------
 */
void Widget::openFile(){
    static IplImage *_srcimg = NULL;
    QFileDialog dlg;
    QString _imgpath;
    _imgpath = dlg.getOpenFileName(this,"Open","",tr("Image　Files(*.jpg　*.png *.bmp)"));
    if(_imgpath == ""){
        return;
    }
    imgpath = ws2s(_imgpath.toStdWString());
    if(_srcimg){
        cvReleaseImage(&_srcimg);
    }
    _srcimg = cvLoadImage(imgpath.data());
    if(_srcimg){
        srcImg->setimg(_srcimg);
    }else{
        QMessageBox dlg;
        dlg.setText("<p><b>ERROR:</b></p> <p>cannot read this image!</p>");
        dlg.exec();
    }
}

void Widget::saveFile(){
    QFileDialog dlg;
    QString _imgpath;
    _imgpath = dlg.getSaveFileName(this,"save image","",tr("Image　Files(*.jpg)"));
    if(_imgpath == ""){
        return;
    }
    string savepath = ws2s(_imgpath.toStdWString());
    if(qrh->getdata() != NULL){
        cvSaveImage(savepath.data(),qrh->getdata());
    }
}

void Widget::startProcess(){
    encodemsg = ws2s(intext->toPlainText().toStdWString() );
    if(encodemsg == ""){
        QMessageBox dlg;
        dlg.setText("<p><b>ERROR:</b></p> <p>Please imput encode string!</p>");
        dlg.exec();
        return;
    }
    if(imgpath == ""){
        QMessageBox dlg;
        dlg.setText("<p><b>ERROR:</b></p> <p>Please select an image!</p>");
        dlg.exec();
        return;
    }
    if(encodemsg != "" && imgpath != ""){
        qrh->feed(imgpath, encodemsg);
    }
}

void Widget::processFinished(){
    dstImg->setimg(qrh->getdata());
}

void Widget::exitsw(){
    exit(0);
}

void Widget::docWindow(){
    QMessageBox dlg;
    dlg.setText("<h1>Welcome to Visual QR code render</h1>\
                <hr />\
                <p>This software will read an image and embed it in a QR code. The goal of this software is to make QR code meaningful for a human without decoding. And as a side effect, the origin OR code will become more beautiful.</p>\
                <p>Please type the string that you want it encoded into text input box, then click File-open to open a image. Origin image will be shown in left image box. And then click start button to start render. For long strings, seconds or even minites is need to generate the visual QR code, So please wait with patience. When the procedure is finished, the final QR code is shown in right image box.</p>\
                <p>To <strong>save</strong> the generated QR code, please click File-save, and choose a path and name to save.</p>\
                <p>To <strong>print</strong> the QR code directly, please click the small print button the top-right of the image box.   </p>\
");
    dlg.exec();
}

void Widget::aboutWindow(){
    QMessageBox dlg;
    dlg.setText("<h1>Visual QR code render</h1>\
    <p>This software is a simple implementation of a new method generating visual halftone QR code, in which Direct Binary Search is used.</p>\
    <p>Please DO NOT use it for any commercial purpose.</p>\
    <p>You can contact us:</p>\
    <p>Ling Dai<br>\
    Email: <a href=\"mailto:qzyz_dailing@163.com\">qzyz_dailing@163.com</a></p>\
    <p>Hao Fu<br>\
    Email: <a href=\"mailto:fuhao@sjtu.edu.cn\">fuhao@sjtu.edu.cn</a></p>");
    dlg.exec();
}

Widget::~Widget()
{
    // NOTICE delete object in a typology order
    delete srcImg;
    delete dstImg;
    delete srcScene;
    delete dstScene;
    delete intext;
    delete htextSplitter;
    delete h1Splitter;
    delete vSplitter;
}
