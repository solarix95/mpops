#ifndef RENDERER_H
#define RENDERER_H

#include <QObject>
#include "movie.h"

class Renderer : QObject
{
    Q_OBJECT
public:
    Renderer(Movie *movie);

    void render();

signals:
    void renderStart();
    void renderProgress(int proc); // 0..100
    void renderEnd();

protected:
    // Subclassing:
    void setProgress(int proc);
    void setFinished();
    virtual void beginRender() = 0;
    Movie *movie();

private:
    Movie *mMovie;
};

#endif // RENDERER_H
