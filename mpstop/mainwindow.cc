#include <QFileDialog>
#include <QImageReader>
#include <QByteArray>
#include <QMenu>
#include <QAction>
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
    connect(ui->btnChooseOutDir, SIGNAL(clicked()), this, SLOT(selectOutDir()));

    mIsInitialized = false;
    mMovie = new Movie();
    mScene = new MovieScene(this, mMovie,&mSelections);

    ui->timeLineView->setScene(mScene);
    ui->cinema->setMovie(mMovie);
    ui->cinema->playLoop(mSettings.playerLoop());

    connect(ui->btnPlayFwd, SIGNAL(clicked()), ui->cinema, SLOT(playFwd()));
    connect(ui->btnPlayBack, SIGNAL(clicked()), ui->cinema, SLOT(playBack()));
    connect(ui->btnPause, SIGNAL(clicked()), ui->cinema, SLOT(pause()));
    connect(ui->cinema, SIGNAL(currentFrame(int)), this, SLOT(updateFrameIndex(int)));
    connect(ui->cinema, SIGNAL(currentFrame(int)), &mSelections, SLOT(select(int)));
    connect(ui->edtCurrentFrame,SIGNAL(valueChanged(int)), ui->cinema, SLOT(setFrame(int)));

    connect(&mSelections, SIGNAL(selected(int)), ui->cinema, SLOT(setFrame(int)));
    connect(&mThread, SIGNAL(thumbCreated(int,qint64,QImage)), mMovie, SLOT(jobThumb(int,qint64,QImage)));
    connect(&mThread, SIGNAL(rendered(int,qint64,QImage)), mMovie, SLOT(jobRender(int,qint64,QImage)));
    connect(&mThread,SIGNAL(movieCompleteRendered()), mMovie, SLOT(setMovieIsComplete()));

    connect(mMovie, SIGNAL(isComplete(bool)), this, SLOT(updateRenderButton(bool)));
    connect(mMovie, SIGNAL(fpsChanged(int)), ui->edtVideoFps, SLOT(setValue(int)));
    connect(mMovie, SIGNAL(frameDeleted(int)), &mSelections, SLOT(removed(int)));
    connect(ui->edtVideoFps, SIGNAL(valueChanged(int)), mMovie, SLOT(setFps(int)));

    connect(ui->chkLoop, SIGNAL(clicked(bool)), ui->cinema, SLOT(playLoop(bool)));

    ui->edtVideoWidth->setValue(mSettings.defaultVideoWidth());
    ui->edtVideoHeight->setValue(mSettings.defaultVideoHeight());
    ui->edtOutDir->setText(mSettings.lastOutDir());

    connect(ui->edtVideoHeight, SIGNAL(valueChanged(int)),&mSettings, SLOT(setDefaultVideoHeight(int)));
    connect(ui->edtVideoWidth , SIGNAL(valueChanged(int)),&mSettings, SLOT(setDefaultVideoWidth(int)));
    connect(ui->edtVideoFps   , SIGNAL(valueChanged(int)),&mSettings, SLOT(setDefaultVideoFps(int)));
    connect(ui->edtVideoHeight, SIGNAL(valueChanged(int)), mMovie, SLOT(setVideoHeight(int)));
    connect(ui->edtVideoWidth, SIGNAL(valueChanged(int)), mMovie, SLOT(setVideoWidth(int)));
    connect(ui->chkLoop, SIGNAL(clicked(bool)), &mSettings, SLOT(setPlayerLoop(bool)));

    mThread.start(mMovie);

    setupMenu();
    QTimer::singleShot(1,this, SLOT(restoreWindowState()));

}

MainWindow::~MainWindow()
{
    mSettings.setLastMainPos(pos());
    mSettings.setLastMainSize(size());
    mSettings.setLastGeometry(geometry());

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

    QStringList files = QFileDialog::getOpenFileNames(this,"Select Images",mSettings.lastInDir(),filter);
    if (files.count() > 0) {
        mMovie->addFrames(files);
        mSettings.setLastInDir(files.first());
    }
}

void MainWindow::selectOutDir()
{
    QString outDir = QFileDialog::getExistingDirectory(this,tr("Select output directory"),mSettings.lastOutDir());
    if (!outDir.isEmpty()) {
        mSettings.setLastOutDir(outDir);
        ui->edtOutDir->setText(outDir);
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
    ui->edtVideoHeight->setValue(mSettings.defaultVideoHeight());
    ui->edtVideoWidth->setValue(mSettings.defaultVideoWidth());
    ui->edtVideoFps->setValue(mSettings.defaultVideoFps());
    ui->chkLoop->setChecked(mSettings.playerLoop());
    mMovie->setVideoHeight(mSettings.defaultVideoHeight());
    mMovie->setVideoWidth(mSettings.defaultVideoWidth());
}

void MainWindow::saveProject()
{
}

void MainWindow::saveAsProject()
{
}

void MainWindow::newProject()
{
    ui->cinema->pause();
    mMovie->clear();
}

void MainWindow::setupMenu()
{
   QMenu   *fileMenu = menuBar()->addMenu(tr("&File"));
   QAction *a;
   a = fileMenu->addAction("&New");
   connect(a, SIGNAL(triggered()), this, SLOT(newProject()));
   a = fileMenu->addAction("&Save");
   connect(a, SIGNAL(triggered()), this, SLOT(saveProject()));
   a->setEnabled(false);
   a = fileMenu->addAction("&SaveAs");
   connect(a, SIGNAL(triggered()), this, SLOT(saveAsProject()));
   a->setEnabled(false);
}
