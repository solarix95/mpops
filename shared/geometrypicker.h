#ifndef GEOMETRYPICKER_H
#define GEOMETRYPICKER_H

#include <QDialog>
#include <QImage>
#include "ui_geometrypicker.h"
#include "imageview.h"

class GeometryPicker : public QDialog
{
    Q_OBJECT
public:
    explicit GeometryPicker(QImage *img,const QString &title, QWidget *parent = 0);
    
    QRect selection() const;

signals:
    
private slots:
    void newSelection(const QRect &rect);
    void edited();
    void ratioRulesChanged();

private:
    void init();

    Ui::DiaGeometryPicker   ui;
    QImage               *mImg;
    ImageView             mImageView;
    QRect                 mLastSelection;

};

#endif // GEOMETRYPICKER_H
