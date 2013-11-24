#include "geometrypicker.h"

//---------------------------------------------------------------
GeometryPicker::GeometryPicker(QImage *img, const QString &title, QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    ui.imageFrame->layout()->addWidget(&mImageView);
    mImg = img;
    if (!title.isEmpty())
        setWindowTitle(title);
    connect(&mImageView, SIGNAL(selection(QRect)), this, SLOT(newSelection(QRect)));
    connect(ui.edtX, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtY, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtWidth, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.edtHeight, SIGNAL(editingFinished()), this, SLOT(edited()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui.btnOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.chkOrgRatio, SIGNAL(stateChanged(int)), this, SLOT(ratioRulesChanged()));
    connect(ui.chkFreezeRatio, SIGNAL(stateChanged(int)), this, SLOT(ratioRulesChanged()));
    init();
}

//---------------------------------------------------------------
QRect GeometryPicker::selection() const
{
    return mLastSelection;
}

//---------------------------------------------------------------
void GeometryPicker::newSelection(const QRect &rect)
{
    mLastSelection = rect;
    ui.edtX->setText(QString("%1").arg(rect.x()));
    ui.edtY->setText(QString("%1").arg(rect.y()));
    ui.edtWidth->setText(QString("%1").arg(rect.width()));
    ui.edtHeight->setText(QString("%1").arg(rect.height()));
}

//---------------------------------------------------------------
void GeometryPicker::edited()
{
    QRect newSelection;
    newSelection.setX(ui.edtX->text().toInt());
    newSelection.setY(ui.edtY->text().toInt());
    newSelection.setWidth(ui.edtWidth->text().toInt());
    newSelection.setHeight(ui.edtHeight->text().toInt());
    mImageView.setSelectedRect(newSelection);
}

//---------------------------------------------------------------
void GeometryPicker::ratioRulesChanged()
{
    mImageView.setKeepOriginalRatio(ui.chkOrgRatio->checkState() == Qt::Checked);
    mImageView.setFreezeRatio(ui.chkFreezeRatio->checkState() == Qt::Checked);
}

//---------------------------------------------------------------
void GeometryPicker::init()
{
    if (!mImg)
        return;
    mImageView.setImage(mImg);
}
