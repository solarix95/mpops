#include <QTimer>
#include "tocrenderer.h"

// -----------------------------------------------------------
TocRenderer::TocRenderer(Movie *movie)
   : Renderer(movie)
{
    mFrameIndex = 0;
    connect(this, SIGNAL(requestNextFrame()), this, SLOT(renderFrame()), Qt::QueuedConnection);
}

// -----------------------------------------------------------
void TocRenderer::setOutDir(const QString &outDir)
{

}

// -----------------------------------------------------------
void TocRenderer::beginRender()
{
    emit requestNextFrame();
}

// -----------------------------------------------------------
void TocRenderer::renderFrame()
{
    Q_ASSERT(mFrameIndex >= 0);
    if (mFrameIndex >= movie()->frameCount()) {
        mTocFile.setFilename("/tmp/test.toc");
        mTocFile.save();
        setFinished();
    } else {
        QImage *frame = movie()->rendered(mFrameIndex);
        Q_ASSERT(frame);
        QString nextFrameName = QString("/tmp/frame_%1.png").arg(mFrameIndex);
        frame->save(nextFrameName);
        mTocFile.appendImage(nextFrameName);

        mFrameIndex++;
        setProgress(100*mFrameIndex/(double)movie()->frameCount());
        emit requestNextFrame();
    }
}
