#ifndef TOCRENDERER_H
#define TOCRENDERER_H

#include "renderer.h"
#include "shared/cinelerratoc.h"

/* ---------------------------------------------------------*\
           TOC-Renderer: renders png-series with
           cinelerra toc ("table of contents") - files.
\* ---------------------------------------------------------*/
class TocRenderer : public Renderer
{
    Q_OBJECT
public:
    explicit TocRenderer(Movie *movie);
    
    void setOutDir(const QString &outDir);

signals:
    void requestNextFrame();
    
public slots:

protected:
    virtual void beginRender();

private slots:
    virtual void renderFrame();

private:

    CinelerraToc mTocFile;
    QString      mOutDir;
    int          mFrameIndex;
};

#endif // TOCRENDERER_H
