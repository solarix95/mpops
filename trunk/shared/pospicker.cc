#include "pospicker.h"

//---------------------------------------------------------------
PosPicker::PosPicker(QImage *img, const QString &title, QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    ui.imageFrame->layout()->addWidget(&mImageView);
    mImg = img;
    if (!title.isEmpty())
        setWindowTitle(title);

    connect(&mImageView, SIGNAL(reference(QPoint)), this, SLOT(newReference(QPoint)));

    /*
    connect(ui.edtX, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtY, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtWidth, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtHeight, SIGNAL(editingFinished()), this, SLOT(edited()));
    */
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    init();
}

//---------------------------------------------------------------
QList<QPoint> PosPicker::references() const
{
    return mReferences;
}

//---------------------------------------------------------------
void PosPicker::newReference(const QPoint &pos)
{
    QString posText;
    if (!mReferences.isEmpty())
        posText = ui.lblValues->text() + " + ";

    mReferences << pos;
    ui.lblValues->setText(posText + QString("%1/%2").arg(pos.x()).arg(pos.y()));
}

//---------------------------------------------------------------
void PosPicker::init()
{
    if (!mImg)
        return;
    mImageView.setImage(mImg);
}
