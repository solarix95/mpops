#include <QFileDialog>
#include <QImageReader>
#include <QByteArray>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "movie.h"
#include "moviescene.h"
#include "cinema.h"
#include "tocrenderer.h"
#include "utils.h"

#define MY_TITLE   "Darksuit Stopmotion"

// -----------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(openFrames()));
    connect(ui->btnChooseOutDir, SIGNAL(clicked()), this, SLOT(selectOutDir()));

    mIsInitialized = false;
    mRenderer = NULL;
    mRenderSplash = NULL;
    mMovie    = new Movie();
    mScene    = new MovieScene(this, mMovie,&mSelections);

    ui->timeLineView->setScene(mScene);
    ui->cinema->setMovie(mMovie);
    ui->cinema->playLoop(mSettings.playerLoop());

    connect(ui->btnPlayFwd, SIGNAL(clicked()), ui->cinema, SLOT(playFwd()));
    connect(ui->btnPlayBack, SIGNAL(clicked()), ui->cinema, SLOT(playBack()));
    connect(ui->btnPause, SIGNAL(clicked()), ui->cinema, SLOT(pause()));
    connect(ui->btnRender, SIGNAL(clicked()), this, SLOT(render()));
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
    connect(mMovie, SIGNAL(dirtyChanged()), this, SLOT(dirtyChanged()));
    connect(mMovie, SIGNAL(requestFileName(QString*)),this, SLOT(selectProjectName(QString*)));
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

// -----------------------------------------------------------
MainWindow::~MainWindow()
{
    mSettings.setLastMainPos(pos());
    mSettings.setLastMainSize(size());
    mSettings.setLastGeometry(geometry());

    if (mRenderer)
        delete mRenderer;

    delete ui;
    mThread.quit();
    mThread.wait();
}

// -----------------------------------------------------------
void MainWindow::openFrames()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QString filter;
    foreach(QByteArray format, formats) {
        filter += " *." + format;
    }
    filter = QString("Images (%1)").arg(filter.trimmed());

    QStringList files = QFileDialog::getOpenFileNames(this,"Select Images",mSettings.lastInDir(),filter,NULL,QFileDialog::DontUseNativeDialog);
    if (files.count() > 0) {
        mMovie->addFrames(files);
        mSettings.setLastInDir(files.first());
    }
}

// -----------------------------------------------------------
void MainWindow::selectOutDir()
{
    QString outDir = QFileDialog::getExistingDirectory(this,tr("Select output directory"),mSettings.lastOutDir());
    if (!outDir.isEmpty()) {
        mSettings.setLastOutDir(outDir);
        ui->edtOutDir->setText(outDir);
    }
}

// -----------------------------------------------------------
void MainWindow::selectProjectName(QString *name)
{
    static QString lastDir;
    *name = QFileDialog::getSaveFileName(this,tr("Projectname"),lastDir);
    lastDir = *name;
}

// -----------------------------------------------------------
void MainWindow::render()
{
    Q_ASSERT(!mRenderer);
    setupTocRenderer(); // TODO: multiple renderer?
    Q_ASSERT(mRenderer);
    connect(mRenderer, SIGNAL(renderStart()), this, SLOT(beginRender()));
    connect(mRenderer, SIGNAL(renderEnd()), this, SLOT(endRender()));
    connect(mRenderer, SIGNAL(renderProgress(int)), this, SLOT(renderProgress(int)));
    mRenderer->render();
}

// -----------------------------------------------------------
void MainWindow::updateFrameIndex(int frm)
{
    ui->edtCurrentFrame->setValue(frm);
}

// -----------------------------------------------------------
void MainWindow::updateRenderButton(bool isEnabled)
{
    ui->btnRender->setEnabled(isEnabled);
}

// -----------------------------------------------------------
void MainWindow::updateRenderSize(QSize toSize)
{
    ui->edtVideoHeight->setValue(toSize.height());
    ui->edtVideoWidth->setValue(toSize.width());
}

// -----------------------------------------------------------
void MainWindow::restoreWindowState()
{

    setGeometry(mSettings.lastGeometry()); // WTF.. not working..?!?
    ui->edtVideoHeight->setValue(mSettings.defaultVideoHeight());
    ui->edtVideoWidth->setValue(mSettings.defaultVideoWidth());
    ui->edtVideoFps->setValue(mSettings.defaultVideoFps());
    ui->chkLoop->setChecked(mSettings.playerLoop());
    mMovie->setVideoHeight(mSettings.defaultVideoHeight());
    mMovie->setVideoWidth(mSettings.defaultVideoWidth());
}

// -----------------------------------------------------------
void MainWindow::saveProject()
{
    Q_ASSERT(mMovie);
    mMovie->save();
}

// -----------------------------------------------------------
void MainWindow::saveAsProject()
{
    static QString lastDir;
    QString tocFile = QFileDialog::getSaveFileName(this,tr("New Cinerella/Blacksuit-TOC"),lastDir,"*.toc");
    if (tocFile.isEmpty())
        return;
    lastDir = tocFile;
    if (!tocFile.endsWith(".toc"))
        tocFile += ".toc";

    Q_ASSERT(mMovie);
    mMovie->saveAs(tocFile);
}

// -----------------------------------------------------------
void MainWindow::newProject()
{
    ui->cinema->pause();
    mMovie->clear();
}

// -----------------------------------------------------------
void MainWindow::openProject()
{
    QString tocFile = QFileDialog::getOpenFileName(this,tr("Select Cinerella/Blacksuit-TOC"),"","*.toc");
    if (tocFile.isEmpty())
        return;
    Q_ASSERT(mMovie);
    mMovie->open(tocFile);
}

// -----------------------------------------------------------
void MainWindow::insertFromClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    if (!clipboard->mimeData() || !mMovie)
        return;

    QList<QByteArray> fileUrls = Darksuit::filesFromMimeData(clipboard->mimeData());
    foreach(QByteArray nextFile, fileUrls) {
        mMovie->addFrame(nextFile);
    }
}

// -----------------------------------------------------------
void MainWindow::revertInsertFromClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    if (!clipboard->mimeData() || !mMovie)
        return;

    QList<QByteArray> fileUrls = Darksuit::filesFromMimeData(clipboard->mimeData());
    for (int i = fileUrls.count() - 1; i >= 0; i--) {
        mMovie->addFrame(fileUrls[i]);
    }
}

// -----------------------------------------------------------
void MainWindow::beginRender()
{
    Q_ASSERT(!mRenderSplash);
    mRenderSplash = new QSplashScreen(this,QPixmap(":/images/splash.png"));
    mRenderSplash->show();
    mRenderSplash->showMessage(" Rendering..",Qt::AlignLeft, Qt::white);
    while (qApp->hasPendingEvents())
        qApp->processEvents();
}

// -----------------------------------------------------------
void MainWindow::renderProgress(int proc)
{
    Q_ASSERT(mRenderSplash);
    mRenderSplash->showMessage(QString(" progress: %1%").arg(proc), Qt::AlignLeft, Qt::white);
}

// -----------------------------------------------------------
void MainWindow::endRender()
{
    Q_ASSERT(mRenderSplash);
    delete mRenderSplash;
    mRenderSplash = NULL;
    delete mRenderer;
    mRenderer = NULL;
}

// -----------------------------------------------------------
void MainWindow::dirtyChanged()
{
    Q_ASSERT(mMovie);
    setWindowTitle(QString("%1 - %2%3").arg(MY_TITLE).arg(mMovie->name()).arg(mMovie->isDirty() ? "*":""));
}

// -----------------------------------------------------------
void MainWindow::setupMenu()
{
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAsProject()));

    connect(ui->actionInsertRevert, SIGNAL(triggered()), this, SLOT(revertInsertFromClipboard()));
}

// -----------------------------------------------------------
void MainWindow::setupTocRenderer()
{
    Q_ASSERT(!mRenderer);

    TocRenderer *tocRenderer = new TocRenderer(mMovie);
    tocRenderer->setOutDir(ui->edtOutDir->text());
    tocRenderer->setFps(ui->edtVideoFps->value());
    mRenderer = tocRenderer;
}
