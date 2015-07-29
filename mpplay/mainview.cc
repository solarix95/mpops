#include <QApplication>
#include <QStringList>
#include "mainview.h"
#include "ui_mainview.h"

#define MY_TITLE "MPPlay - %1"

//-----------------------------------------------------------------------------
MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    mSource = NULL;
    mBuffer = new VideoBuffer();
    ui->VideoWidget->setBuffer(mBuffer);

    QStringList args = qApp->arguments();
    args.takeFirst();

    if (args.count() > 0) {
        mSourceName = args.takeFirst();

        mSource = new VideoSource();
        connect(mSource,SIGNAL(started()), this, SLOT(sourceReady()));
        mSource->start();
    }

    ui->VideoWidget->setFocus();
}

//-----------------------------------------------------------------------------
MainView::~MainView()
{
    delete ui;
    if (mBuffer)
        delete mBuffer;
    if (mSource) {
        mSource->quit();
        mSource->wait(5000);
        delete mSource;
    }
}

//-----------------------------------------------------------------------------
void MainView::sourceReady()
{
    Q_ASSERT(mSource);
    Q_ASSERT(!mSourceName.isEmpty());

    setWindowTitle(QString(MY_TITLE).arg(mSourceName));
    mBuffer->setSource(mSource);
    mSource->load(mSourceName);
    ui->timeline->setBuffer(mBuffer);
}
