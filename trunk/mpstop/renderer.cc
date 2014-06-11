#include "renderer.h"

// -----------------------------------------------------------
Renderer::Renderer(Movie *movie)
{
    Q_ASSERT(movie);
    mMovie = movie;
}

// -----------------------------------------------------------
void Renderer::render()
{
    emit renderStart();
    beginRender();
}

// -----------------------------------------------------------
void Renderer::setProgress(int proc)
{
    Q_ASSERT(proc >= 0 && proc <= 100);
    emit renderProgress(proc);
}

// -----------------------------------------------------------
void Renderer::setFinished()
{
    emit renderEnd();

}

// -----------------------------------------------------------
Movie *Renderer::movie()
{
    return mMovie;
}
