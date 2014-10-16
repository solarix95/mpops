#ifndef IMAGERECTVIEW_H
#define IMAGERECTVIEW_H

#include <QWidget>

class ImageRectView : public QWidget
{
    Q_OBJECT
public:
    explicit ImageRectView(const QImage *img = NULL, QWidget *parent = 0);
    
    void setImage(const QImage *img);
    void setReferenceSelection(const QRect &rect);
    virtual bool event(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual QSize sizeHint() const;

    void  setKeepOriginalRatio(bool keepRatio);
    void  setFreezeRatio(bool freeze);

    void  setSelectedRect(const QRect &newRect);
    QRect selectedRect(bool *hasSelection) const;

signals:
    void selection(const QRect &rect);

private:
    void calculateImageSelection();
    QRect movingRect() const;
    int   mouseX(QMouseEvent *e);
    int   mouseY(QMouseEvent *e);
    QRect positiveWidgetSelection() const;
    QRect toWidgetRect(const QRect &rect) const;
    void  adjustRatio();
    bool  hasFreezedRatio() const;


    const QImage *mOriginalImg;
    QImage        mScaledImage;
    QRect         mWidSel;
    QRect         mImgSel;
    QRect         mReference;
    QPoint        mMoveStart;
    QPoint        mMoveEnd;
    bool          mKeepOriginalRatio;
    float         mFreezedRatio;

    typedef  enum eMode {
        Idle,
        Creating,
        Expanding,
        Moving
    } Mode;

    Mode  mMode;
};

#endif // IMAGEVIEW_H
