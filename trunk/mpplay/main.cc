

#include <QMetaType>
#include <QApplication>

#include "shared/image.h"
#include "mainview.h"


//---------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    qRegisterMetaType<ImagePtr>("ImagePtr");

    MainView view;
    view.show();

    return app.exec();
}
