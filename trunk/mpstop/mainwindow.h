#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "selectionmodel.h"
#include "workerthread.h"
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
    void updateFrameIndex(int frm);
    void updateRenderButton(bool isEnabled);
    void restoreWindowState();

private:
    Ui::MainWindow *ui;

    Movie          *mMovie;
    MovieScene     *mScene;
    DefSettings     mSettings;
    SelectionModel  mSelections;
    WorkerThread    mThread;
    bool            mIsInitialized;

};

#endif // MAINWINDOW_H
