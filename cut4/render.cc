#include <QApplication>
#include <QTime>
#include <QDebug>
#include "render.h"
#include "shape/c4shape.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
Render::Render(C4Scene *scene, QWidget *parent) :
    QWidget(parent), mScene(scene)
{
    ui.setupUi(this);
    connect(ui.btnRender, SIGNAL(clicked()), this, SLOT(render()));

    connect(mScene, SIGNAL(renderingStarted()), this, SLOT(renderingStarted()));
    connect(mScene, SIGNAL(renderingEnded()), this, SLOT(renderingEnded()));
    connect(mScene, SIGNAL(rendered(int,QImage&)), this, SLOT(renderingDone(int, QImage&)));
    connect(ui.slider3d, SIGNAL(valueChanged(int)), this, SLOT(updateSettings()));
    connect(ui.sliderEye, SIGNAL(valueChanged(int)), this, SLOT(updateSettings()));
    updateSettings();
}

//-------------------------------------------------------------------------------------------------
void Render::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

//-------------------------------------------------------------------------------------------------
void Render::render()
{
    // mScene->singleShot();
    mScene->render(ui.spbFromFrame->value(), ui.spbToFrame->value());
}

//-------------------------------------------------------------------------------------------------
void  Render::renderingStarted()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    while (qApp->hasPendingEvents())
        qApp->processEvents();
}

//-------------------------------------------------------------------------------------------------
void  Render::renderingDone(int index, QImage &img)
{
    while (qApp->hasPendingEvents())
        qApp->processEvents();
}

//-------------------------------------------------------------------------------------------------
void  Render::renderingEnded()
{
    QApplication::restoreOverrideCursor();
}

//-------------------------------------------------------------------------------------------------
void Render::updateSettings()
{
    ui.spbEye->setValue(ui.sliderEye->value()/5.0);
    ui.spb3d->setValue(ui.slider3d->value()/100.0);
}

//-------------------------------------------------------------------------------------------------
void Render::applySettings()
{
    Q_ASSERT(mScene);
    mScene->setAttribut(C4Scene::StereoEffect,ui.slider3d->value()/100.0);
    mScene->setAttribut(C4Scene::StereoEyeSep,ui.sliderEye->value()/5.0);
}


