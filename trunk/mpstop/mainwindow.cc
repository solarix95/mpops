#include <QFileDialog>
#include <QImageReader>
#include <QByteArray>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "movie.h"
#include "moviescene.h"
#include "cinema.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openFrames()));

    mMovie = new Movie();
    mScene = new MovieScene(this, mMovie,&mSelections);

    ui->timeLineView->setScene(mScene);
    ui->cinema->setMovie(mMovie);

    connect(&mSelections, SIGNAL(selected(int)), ui->cinema, SLOT(setFrame(int)));
    connect(&mThread, SIGNAL(thumbCreated(int,qint64,QImage)), mMovie, SLOT(jobThumb(int,qint64,QImage)));
    connect(&mThread, SIGNAL(rendered(int,qint64,QImage)), mMovie, SLOT(jobRender(int,qint64,QImage)));

    mThread.start(mMovie);
}

MainWindow::~MainWindow()
{
    delete ui;
    mThread.quit();
    mThread.wait();
}

void MainWindow::openFrames()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QString filter;
    foreach(QByteArray format, formats) {
        filter += " *." + format;
    }
    filter = QString("Images (%1)").arg(filter.trimmed());

    QStringList files = QFileDialog::getOpenFileNames(this,"Select Images",QString(),filter);
    if (files.count() > 0)
        mMovie->addFrames(files);

    qDebug() << "FERTIG";
}
