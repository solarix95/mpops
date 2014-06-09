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

    mIsInitialized = false;
    mMovie = new Movie();
    mScene = new MovieScene(this, mMovie,&mSelections);

    ui->timeLineView->setScene(mScene);
    ui->cinema->setMovie(mMovie);

    connect(ui->btnPlayFwd, SIGNAL(clicked()), ui->cinema, SLOT(playFwd()));
    connect(ui->btnPlayBack, SIGNAL(clicked()), ui->cinema, SLOT(playBack()));
    connect(ui->btnPause, SIGNAL(clicked()), ui->cinema, SLOT(pause()));
    connect(ui->cinema, SIGNAL(currentFrame(int)), this, SLOT(updateFrameIndex(int)));
    connect(ui->edtCurrentFrame,SIGNAL(valueChanged(int)), ui->cinema, SLOT(setFrame(int)));

    connect(&mSelections, SIGNAL(selected(int)), ui->cinema, SLOT(setFrame(int)));
    connect(&mThread, SIGNAL(thumbCreated(int,qint64,QImage)), mMovie, SLOT(jobThumb(int,qint64,QImage)));
    connect(&mThread, SIGNAL(rendered(int,qint64,QImage)), mMovie, SLOT(jobRender(int,qint64,QImage)));
    connect(&mThread,SIGNAL(movieCompleteRendered()), mMovie, SLOT(setMovieIsComplete()));

    connect(mMovie, SIGNAL(isComplete(bool)), this, SLOT(updateRenderButton(bool)));


    ui->edtVideoWidth->setValue(mSettings.defaultWidth());
    ui->edtVideoHeight->setValue(mSettings.defaultHeight());
    mThread.start(mMovie);

    QTimer::singleShot(1,this, SLOT(restoreWindowState()));
}

MainWindow::~MainWindow()
{
    qDebug() << "SAVE" << pos();
    mSettings.setLastMainPos(pos());
    mSettings.setLastMainSize(size());
    mSettings.setLastGeometry(geometry());

    delete ui;
    mThread.quit();
    mThread.wait();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Polish && !mIsInitialized) {
    }
    return QMainWindow::event(event);
}

void MainWindow::openFrames()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QString filter;
    foreach(QByteArray format, formats) {
        filter += " *." + format;
    }
    filter = QString("Images (%1)").arg(filter.trimmed());

    QStringList files = QFileDialog::getOpenFileNames(this,"Select Images",mSettings.lastInDir(),filter);
    if (files.count() > 0) {
        mMovie->addFrames(files);
        mSettings.setLastInDir(files.first());
    }
}

void MainWindow::updateFrameIndex(int frm)
{
    ui->edtCurrentFrame->setValue(frm);
}

void MainWindow::updateRenderButton(bool isEnabled)
{
    ui->btnRender->setEnabled(isEnabled);
}

void MainWindow::restoreWindowState()
{
    // resize(mSettings.lastMainSize());
    // move(mSettings.lastMainPos());
    qDebug() << pos() << mSettings.lastMainPos();
    setGeometry(mSettings.lastGeometry()); // WTF.. not working..?!?
}
