#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include "ui_mainform.h"

#include <QTimer>
#include <QImage>
#include "objfile.h"
#include "render/c4scene.h"


class Mainview : public QWidget
{
    Q_OBJECT
public:
    explicit Mainview(C4Scene *scene, QWidget *parent = 0);
    
signals:
    void renderRequest(int frame);

 protected:
    virtual void closeEvent(QCloseEvent *);

private slots:
    void render();
    void renderingStarted();
    void renderingDone(int index, QImage &img);
    void renderingEnded();

private:
    Ui::Widget ui;
    C4Scene   *mScene;
};

#endif // MAINVIEW_H
