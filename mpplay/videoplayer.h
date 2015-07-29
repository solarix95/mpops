#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QTimer>
#include "videobuffer.h"
#include "shared/image.h"

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = 0);

    enum State {
        Undef,   // no source..
        Busy,    // loading/buffering
        Playing,
        Pause,
        Ended
    };
    void setBuffer(VideoBuffer *buffer);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void startPlay();  // start-Request by Buffer
    void stopPlay();   // stop-Request by Buffer

    void bufferStateChanged(int state);
    void grabNextFrame();
    
private:
    void setState(State st);

    VideoBuffer *mBuffer;

    QTimer       mTimer;
    ImagePtr     mCurrentFrame;
    int          mPlayFps;
    State        mState;
};

#endif // VIDEOPLAYER_H
