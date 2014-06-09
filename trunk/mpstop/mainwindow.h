#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "selectionmodel.h"
#include "workerthread.h"

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

private:
    Ui::MainWindow *ui;

    Movie          *mMovie;
    MovieScene     *mScene;
    Cinema         *mCinema;
    SelectionModel  mSelections;
    WorkerThread    mThread;

};

#endif // MAINWINDOW_H
