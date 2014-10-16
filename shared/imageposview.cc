#include <limits>
#include <math.h>
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>
#include "imageposview.h"
#include <QDebug>

//---------------------------------------------------------------
ImagePosView::ImagePosView(const QImage *img,QWidget *parent) :
    QWidget(parent)
{
    mOriginalImg       = NULL;
    setImage(img);
}

//---------------------------------------------------------------
bool ImagePosView::event(QEvent *e)
{

    QMouseEvent *me = dynamic_cast<QMouseEvent*>(e);
    int mx = mouseX(me);
    int my = mouseY(me);

    if (e->type() == QEvent::MouseButtonPress) {
        Q_ASSERT(me);
        if (me->button() == Qt::LeftButton) {
            mClickStart = QPoint(mx,my);
        }
    }

    if (e->type() == QEvent::MouseButtonRelease) {
        Q_ASSERT(me);
        if (me->button() == Qt::LeftButton) {
            mClickEnd = QPoint(mx,my);
            if (mClickStart == mClickEnd)
                emit reference(toImagePos(mClickEnd));
        }
    }

    return QWidget::event(e);
}

//---------------------------------------------------------------
void ImagePosView::resizeEvent(QResizeEvent *e)
{
    if (!mOriginalImg)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mScaledImage = mOriginalImg->scaled(e->size(),Qt::KeepAspectRatio);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------
void ImagePosView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    p.drawImage(0,0,mScaledImage);

    QWidget::paintEvent(e);
}

//---------------------------------------------------------------
QSize ImagePosView::sizeHint() const
{
    return mScaledImage.size();
}

//---------------------------------------------------------------
void ImagePosView::setImage(const QImage *img)
{
    mOriginalImg = img;
    mScaledImage = QImage();
    if (!mOriginalImg)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mScaledImage = mOriginalImg->scaled(size(),Qt::KeepAspectRatio);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------
int ImagePosView::mouseX(QMouseEvent *e)
{
    if (!e)
        return 0;
    int x = e->x() >= 0 ? e->x() : 0;
    return x < width()-1 ? x : width() -1;
}

//---------------------------------------------------------------
int ImagePosView::mouseY(QMouseEvent *e)
{
    if (!e)
        return 0;
    int y = e->y() >= 0 ? e->y() : 0;
    y = y < height()-1 ? y : height() -1;
    return y;
}

//---------------------------------------------------------------
QPoint ImagePosView::toWidgetPos(const QPoint &imagePos) const
{
    float fx = mOriginalImg->width()  / (float)mScaledImage.width();
    float fy = mOriginalImg->height() / (float)mScaledImage.height();
    return QPoint(imagePos.x() / fx, imagePos.y() / fy);
}

//---------------------------------------------------------------
QPoint ImagePosView::toImagePos(const QPoint &widgetPos) const
{
    float fx = mOriginalImg->width()  / (float)mScaledImage.width();
    float fy = mOriginalImg->height() / (float)mScaledImage.height();
    return QPoint(widgetPos.x() * fx, widgetPos.y() * fy);
}
