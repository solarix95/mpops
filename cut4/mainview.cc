#include <QApplication>
#include <QTime>
#include <QDebug>
#include "mainview.h"
#include "shape/c4shape.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
Mainview::Mainview(C4Scene *scene, QWidget *parent) :
    QWidget(parent), mScene(scene)
{
    ui.setupUi(this);
    connect(ui.btnRender, SIGNAL(clicked()), this, SLOT(render()));

    connect(mScene, SIGNAL(renderingStarted()), this, SLOT(renderingStarted()));
    connect(mScene, SIGNAL(renderingEnded()), this, SLOT(renderingEnded()));
    connect(mScene, SIGNAL(rendered(int,QImage&)), this, SLOT(renderingDone(int, QImage&)));

}

//-------------------------------------------------------------------------------------------------
void Mainview::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

//-------------------------------------------------------------------------------------------------
void Mainview::render()
{

    mScene->setAttribut(C4Scene::StereoEffect,ui.slider3d->value()/100.0);
    mScene->setAttribut(C4Scene::StereoEyeSep,ui.sliderEye->value()/5.0);

    /*
    mScene->shape(1)->rotate(ui.sliderX->value(),-ui.sliderY->value(),0);
    mScene->shape(1)->scale(ui.sliderScaleX->value()/10.0, ui.sliderScaleY->value()/10.0, 1);
    mScene->shape(1)->setPos(5,0,ui.sliderZ->value());


    float f = 0;
    mScene->shape(1)->rotate(30.0 * f,30.0 * f,0);
    mScene->shape(1)->setPos(0,0,100 - (50 * f));

    */

    // mScene->singleShot();
    mScene->render();

    /*
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    mScene->setAttribut(C4Scene::StereoEffect,ui.slider3d->value()/100.0);
    mScene->setAttribut(C4Scene::StereoEyeSep,ui.sliderEye->value()/5.0);

    mScene->shape(1)->rotate(ui.sliderX->value(),-ui.sliderY->value(),0);
    mScene->shape(1)->scale(ui.sliderScaleX->value()/10.0, ui.sliderScaleY->value()/10.0, 1);
    mScene->shape(1)->setPos(5,0,ui.sliderZ->value());

    QImage img;
    mScene->render(&img);
    ui.lblView->setPixmap(QPixmap::fromImage(img));

    QApplication::restoreOverrideCursor();
    */

}

//-------------------------------------------------------------------------------------------------
void  Mainview::renderingStarted()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    while (qApp->hasPendingEvents())
        qApp->processEvents();
}

//-------------------------------------------------------------------------------------------------
void  Mainview::renderingDone(int index, QImage &img)
{
    /*
    float f = index / 240.0;
    mScene->setAttribut(C4Scene::StereoEffect,f);

    mScene->shape(1)->rotate(30.0 * f,30.0 * f,0);
    mScene->shape(1)->setPos(0,0,100 - (50 * f));

    */
    ui.lblView->setPixmap(QPixmap::fromImage(img));
    while (qApp->hasPendingEvents())
        qApp->processEvents();

    img.save(QString("/tmp/out/frame_%1.png").arg(index));

}

//-------------------------------------------------------------------------------------------------
void  Mainview::renderingEnded()
{
    QApplication::restoreOverrideCursor();
}


