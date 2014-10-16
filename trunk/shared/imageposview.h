#ifndef IMAGEPOSVIEW_H
#define IMAGEPOSVIEW_H

#include <QWidget>

class ImagePosView : public QWidget
{
    Q_OBJECT
public:
    explicit ImagePosView(const QImage *img = NULL, QWidget *parent = 0);
    
    void setImage(const QImage *img);

    virtual QSize sizeHint() const;

signals:
    void reference(const QPoint &pos);

protected:
    virtual bool event(QEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *);

private:
    int    mouseX(QMouseEvent *e);
    int    mouseY(QMouseEvent *e);
    QPoint toWidgetPos(const QPoint &imagePos) const;
    QPoint toImagePos(const QPoint &widgetPos) const;
    void   adjustRatio();
    bool   hasFreezedRatio() const;


    const QImage *mOriginalImg;
    QImage        mScaledImage;
    QPoint        mClickStart;
    QPoint        mClickEnd;
};

#endif // IMAGEVIEW_H
