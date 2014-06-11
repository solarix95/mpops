#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QSplashScreen>
#include "selectionmodel.h"
#include "workerthread.h"
#include "renderer.h"
#include "defsettings.h"

namespace Ui {
class MainWindow;
}

class Movie;
class MovieScene;
class Cinema;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openFrames();
    void selectOutDir();
    void render();
    void updateFrameIndex(int frm);
    void updateRenderButton(bool isEnabled);
    void restoreWindowState();

    // Menu-Actions:
    void saveProject();
    void saveAsProject();
    void newProject();

    // Render-State:
    void beginRender();
    void renderProgress(int proc);
    void endRender();

private:
    void setupMenu();
    void setupTocRenderer();


    Ui::MainWindow *ui;

    Movie          *mMovie;
    MovieScene     *mScene;
    Renderer       *mRenderer;
    QSplashScreen  *mRenderSplash;
    DefSettings     mSettings;
    SelectionModel  mSelections;
    WorkerThread    mThread;
    bool            mIsInitialized;

};

#endif // MAINWINDOW_H
