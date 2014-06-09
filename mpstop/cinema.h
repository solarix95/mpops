#ifndef CINEMA_H
#define CINEMA_H

#include <QWidget>

class Movie;
class Cinema : public QWidget
{
    Q_OBJECT
public:
    explicit Cinema(QWidget *parent = 0);
    
    void setMovie(Movie *movie);

signals:
    
public slots:
    void setFrame(int index);

protected:
    virtual void paintEvent(QPaintEvent *);

private slots:
    void frameChanged(int frameIndex);

private:
    Movie *mMovie;
    bool   mIsPlaying;
    int    mCurrentFrame;
    
};

#endif // CINEMA_H
