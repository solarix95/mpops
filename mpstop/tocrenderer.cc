#include <QTimer>
#include <QDir>
#include <QInputDialog>
#include "tocrenderer.h"

// -----------------------------------------------------------
TocRenderer::TocRenderer(Movie *movie)
   : Renderer(movie)
{
    mFrameIndex = 0;
    mTocName    = "content";
    connect(this, SIGNAL(requestNextFrame()), this, SLOT(renderFrame()), Qt::QueuedConnection);
}

// -----------------------------------------------------------
void TocRenderer::setOutDir(const QString &outDir)
{
    mOutDir = outDir;
}

// -----------------------------------------------------------
void TocRenderer::setFps(int fps)
{
    mTocFile.setFps(fps);
}

// -----------------------------------------------------------
void TocRenderer::beginRender()
{
    mTocName = QInputDialog::getText(NULL,"TOC","Enter clipname",QLineEdit::Normal, mTocName);
    if (mTocName.isEmpty()) {
        setFinished();
        return;
    }

    emit requestNextFrame();
}

// -----------------------------------------------------------
void TocRenderer::renderFrame()
{
    Q_ASSERT(mFrameIndex >= 0);
    if (mFrameIndex >= movie()->frameCount()) {
        mTocFile.setFilename(mOutDir + QDir::separator() + mTocName +  ".toc");
        mTocFile.save();
        setFinished();
    } else {
        QImage *frame = movie()->rendered(mFrameIndex);
        Q_ASSERT(frame);
        QString nextFrameName = mOutDir + QDir::separator() + QString("frame_%1.png").arg(mFrameIndex);
        frame->save(nextFrameName);
        mTocFile.appendImage(nextFrameName);

        mFrameIndex++;
        setProgress(100*mFrameIndex/(double)movie()->frameCount());
        emit requestNextFrame();
    }
}
