#include "renderer.h"

Renderer::Renderer(Movie *movie)
{
}

void Renderer::render()
{
    emit renderStart();
    beginRender();
}

void Renderer::setProgress(int proc)
{
    Q_ASSERT(proc >= 0 && proc <= 100);

}

void Renderer::setFinished()
{
    emit renderEnd();

}
