#include "resourcewidget.h"
#include "ui_resourceform.h"

//-------------------------------------------------------------------------------------------------
ResourceWidget::ResourceWidget(C4Scene *scene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResourceWidget),
    mScene(scene)
{
    ui->setupUi(this);
}

//-------------------------------------------------------------------------------------------------
ResourceWidget::~ResourceWidget()
{
    delete ui;
}

//-------------------------------------------------------------------------------------------------
void ResourceWidget::store(QSettings *s)
{
    s->beginGroup("ResourceWidget");
    s->setValue("Geometry",geometry());
    s->endGroup();
}

//-------------------------------------------------------------------------------------------------
void ResourceWidget::restore(QSettings *s)
{
    s->beginGroup("ResourceWidget");
    if (s->contains("Geometry"))
        setGeometry(s->value("Geometry").toRect());
    s->endGroup();
}
