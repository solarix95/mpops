#ifndef GEOMETRYPICKER_H
#define GEOMETRYPICKER_H

#include <QDialog>
#include <QImage>
#include <QPoint>
#include <QList>
#include "ui_pospicker.h"
#include "imageposview.h"

class PosPicker : public QDialog
{
    Q_OBJECT
public:
    explicit PosPicker(QImage *img,const QString &title, QWidget *parent = 0);
    
    QList<QPoint> references() const;

signals:
    
private slots:
    void newReference(const QPoint &pos);

private:
    void init();

    Ui::DiaPosPicker      ui;
    QImage               *mImg;
    ImagePosView          mImageView;
    QList<QPoint>         mReferences;
};

#endif // GEOMETRYPICKER_H
