#ifndef CINEMA_H
#define CINEMA_H

#include <QWidget>
#include <QTimer>

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

protected:
    virtual void paintEvent(QPaintEvent *);

private slots:
    void frameChanged(int frameIndex);
    void showNextFrame();

private:
    Movie  *mMovie;
    QTimer  mPlayTimer;
    bool    mIsPlaying;
    int     mCurrentFrame;
    int     mPlayDirection;
    
};

#endif // CINEMA_H
