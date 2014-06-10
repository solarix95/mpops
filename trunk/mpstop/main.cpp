#include <QtGui/QApplication>
#include <QFile>
#include <QString>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Load an application style
    QFile styleFile( ":/darkorange.qss" );
    styleFile.open( QFile::ReadOnly );

    // Apply the loaded stylesheet
    QString style( styleFile.readAll() );
    a.setStyleSheet( style );


    MainWindow w;
    w.show();
    
    return a.exec();
}
