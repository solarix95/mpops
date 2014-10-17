#ifndef CINEMA_H
#define CINEMA_H

#include <QWidget>
#include <QTimer>
#include <QTime>

class Movie;
class Cinema : public QWidget
{
    Q_OBJECT
public:
    explicit Cinema(QWidget *parent = 0);
    
    void setMovie(Movie *movie);

signals:
    void currentFrame(int);
    
public slots:
    void setFrame(int index);

    // Player:
    void playFwd();
    void playBack();
    void pause();
    void playLoop(bool doLoop);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void dropEvent(QDropEvent *);
    virtual void dragEnterEvent(QDragEnterEvent *);

private slots:
    void frameChanged(int frameIndex);
    void showNextFrame();
    void fpsChanged(int newFps);
    void initAnimation();

private:
    Movie  *mMovie;
    QTimer  mPlayTimer;
    bool    mIsPlaying;
    int     mCurrentFrame;
    int     mPlayDirection;
    bool    mLoop;
    QTime   mStartTime;
    
};

#endif // CINEMA_H
