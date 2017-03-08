#include "codepovision.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CodepoVision w;
    w.show();

    return a.exec();
}
