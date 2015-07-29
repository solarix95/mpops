#include <limits>
#include <math.h>
#include <QPainter>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>
#include "imagerectview.h"
#include <QDebug>

//---------------------------------------------------------------
ImageRectView::ImageRectView(const QImage *img,QWidget *parent) :
    QWidget(parent)
{
    mOriginalImg       = NULL;
    mMode              = Idle;
    mKeepOriginalRatio = false;
    setImage(img);
}

//---------------------------------------------------------------
bool ImageRectView::event(QEvent *e)
{

    QMouseEvent *me = dynamic_cast<QMouseEvent*>(e);
    int mx = mouseX(me);
    int my = mouseY(me);

    if (e->type() == QEvent::MouseButtonPress) {
        Q_ASSERT(me);
        if (me->button() == Qt::LeftButton) {
            if (mWidSel.contains(mx,my)) {
                mMoveStart.setX(mx);
                mMoveStart.setY(my);
                mMode = Moving;
                mMoveEnd = mMoveStart;
            } else {
                mWidSel.setX(mx);
                mWidSel.setY(my);
                mWidSel.setWidth(0);
                mWidSel.setHeight(0);
                mMode = Creating;
            }
            calculateImageSelection();
        }
        if (me->button() == Qt::RightButton) {  // Reset
            mWidSel.setX(0);
            mWidSel.setY(0);
            mWidSel.setWidth(0);
            mWidSel.setHeight(0);
            calculateImageSelection();
        }
    }
    if ((mMode != Idle) && (e->type() == QEvent::MouseMove)) {
        Q_ASSERT(me);
        if (mMode == Moving) {
            mMoveEnd.setX(mx);
            mMoveEnd.setY(my);
        } else {
            mWidSel.setWidth(mx - mWidSel.x());
            mWidSel.setHeight(my - mWidSel.y());
            adjustRatio();
        }
        calculateImageSelection();
    }

    if (e->type() == QEvent::MouseButtonRelease) {
        Q_ASSERT(me);
        if (me->button() == Qt::LeftButton) {
            if (mMode == Moving) {
                mWidSel = movingRect();
            }
            mMode = Idle;
            calculateImageSelection();
        }
    }

    return QWidget::event(e);
}

//---------------------------------------------------------------
void ImageRectView::resizeEvent(QResizeEvent *e)
{
    if (!mOriginalImg)
        return;
    mWidSel.setWidth(0);
    mWidSel.setHeight(0);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mScaledImage = mOriginalImg->scaled(e->size(),Qt::KeepAspectRatio);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------
void ImageRectView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    p.drawImage(0,0,mScaledImage);
    p.setPen(Qt::red);
    p.drawRect(mWidSel);

    if (mMode == Moving) {
        p.setPen(Qt::blue);
        p.drawRect(movingRect());
    }

    QRect referenceRect = toWidgetRect(mReference);
    if (referenceRect.isValid()) {
        p.setPen(Qt::green);
        p.drawRect(referenceRect);
    }

    QWidget::paintEvent(e);
}

//---------------------------------------------------------------
QSize ImageRectView::sizeHint() const
{
    return mScaledImage.size();
}

//---------------------------------------------------------------
void ImageRectView::setImage(const QImage *img)
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
void ImageRectView::setReferenceSelection(const QRect &rect)
{
    mReference = rect;
}

//---------------------------------------------------------------
void  ImageRectView::setKeepOriginalRatio(bool keepRatio)
{
    mKeepOriginalRatio = keepRatio;
    adjustRatio();
}

//---------------------------------------------------------------
void  ImageRectView::setFreezeRatio(bool freeze)
{
    mFreezedRatio = freeze ? mWidSel.width() / (float)mWidSel.height() : 0.0;
    adjustRatio();
}

//---------------------------------------------------------------
void  ImageRectView::setSelectedRect(const QRect &newRect)
{
    mImgSel = newRect;

    QRect newWidgetSelection = toWidgetRect(newRect);
    if (newWidgetSelection.isEmpty())
        return;

    mWidSel = newWidgetSelection;

    adjustRatio();
    update();
}

//---------------------------------------------------------------
QRect ImageRectView::selectedRect(bool */*hasSelection*/) const
{
    return mImgSel;
}

//---------------------------------------------------------------
void ImageRectView::calculateImageSelection()
{
    if (!mOriginalImg)
        return;

    QRect widSel = mMode == Moving ? movingRect() : positiveWidgetSelection();

    // Correct coordinates to original image:
    float fx = mOriginalImg->width()  / (float)mScaledImage.width();
    float fy = mOriginalImg->height() / (float)mScaledImage.height();

    mImgSel.setX(widSel.x()*fx);
    mImgSel.setY(widSel.y()*fy);
    mImgSel.setWidth(widSel.width()*fx);
    mImgSel.setHeight(widSel.height()*fy);

    emit selection(mImgSel);
    update();
}

//---------------------------------------------------------------
QRect ImageRectView::movingRect() const
{
    QRect moveRect = positiveWidgetSelection();
    int dx = mMoveEnd.x() - mMoveStart.x();
    int dy = mMoveEnd.y() - mMoveStart.y();
    QPoint topLeft(QPoint(moveRect.x() + dx,moveRect.y() + dy));

    // Test borders:
    if (topLeft.x() < 0)
        topLeft.setX(0);
    if (topLeft.y() < 0)
        topLeft.setY(0);
    int borderX = mScaledImage.width()-1 - (topLeft.x()+moveRect.width());
    if (borderX < 0)
        topLeft.setX(topLeft.x() + borderX);
    int borderY = mScaledImage.height()-1 - (topLeft.y()+moveRect.height());
    if (borderY < 0)
        topLeft.setY(topLeft.y() + borderY);

    moveRect.moveTopLeft(topLeft);
    return moveRect;
}

//---------------------------------------------------------------
int ImageRectView::mouseX(QMouseEvent *e)
{
    if (!e)
        return 0;
    int x = e->x() >= 0 ? e->x() : 0;
    return x < width()-1 ? x : width() -1;
}

//---------------------------------------------------------------
int ImageRectView::mouseY(QMouseEvent *e)
{
    if (!e)
        return 0;
    int y = e->y() >= 0 ? e->y() : 0;
    y = y < height()-1 ? y : height() -1;
    return y;
}

//---------------------------------------------------------------
QRect ImageRectView::positiveWidgetSelection() const
{
    QRect posWidRect;
    posWidRect.setX(mWidSel.width()  > 0 ? mWidSel.x() : mWidSel.x()+mWidSel.width());
    posWidRect.setY(mWidSel.height() > 0 ? mWidSel.y() : mWidSel.y()+mWidSel.height());
    posWidRect.setWidth(mWidSel.width()  > 0 ? mWidSel.width() : -mWidSel.width());
    posWidRect.setHeight(mWidSel.height()  > 0 ? mWidSel.height() : -mWidSel.height());
    return posWidRect;
}

//---------------------------------------------------------------
QRect ImageRectView::toWidgetRect(const QRect &rect) const
{
    QRect ret;
    if (!mOriginalImg)
        return ret;

    float fx = mOriginalImg->width()  / (float)mScaledImage.width();
    float fy = mOriginalImg->height() / (float)mScaledImage.height();

    ret.setX(rect.x()/fx);
    ret.setY(rect.y()/fy);
    ret.setWidth(rect.width()/fx);
    ret.setHeight(rect.height()/fy);
    return ret;
}

//---------------------------------------------------------------
void ImageRectView::adjustRatio()
{
    if (!mOriginalImg || (!mKeepOriginalRatio && !hasFreezedRatio()))
        return;
    float ratio = mKeepOriginalRatio ? mOriginalImg->width() / (float)mOriginalImg->height() : mFreezedRatio;
    mWidSel.setWidth(mWidSel.height() * ratio);
    update();
}

//---------------------------------------------------------------
bool   ImageRectView::hasFreezedRatio() const
{
    return (fabs(mFreezedRatio - 0.0) > std::numeric_limits<float>::epsilon());
}
