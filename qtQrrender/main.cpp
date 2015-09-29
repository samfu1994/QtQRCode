#include "widget.h"
#include <QApplication>

extern int decodeTest();

int main(int argc, char *argv[])
{
//    decodeTest();
//    exit(0);
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}

