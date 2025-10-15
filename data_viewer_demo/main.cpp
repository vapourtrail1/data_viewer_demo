#include <QApplication>
#include "c_ui/MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    CTViewer w;
    w.show();
    return a.exec();
}
