#include "eyeball.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eyeball w;
    w.show();
    return a.exec();
}
