#include "playerwidget.h"
#include "ui_playerform.h"
#include "render/c4scene.h"

//-------------------------------------------------------------------------------------------------
PlayerWidget::PlayerWidget(C4Scene *scene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerForm), mScene(scene)
{
    ui->setupUi(this);
    connect(mScene,SIGNAL(rendered(int,QImage&)), this, SLOT(frameRendered(int,QImage&)));
}

//-------------------------------------------------------------------------------------------------
PlayerWidget::~PlayerWidget()
{
    delete ui;
}

//-------------------------------------------------------------------------------------------------
void PlayerWidget::store(QSettings *s)
{
    s->beginGroup("PlayerWidget");
    s->setValue("Geometry",geometry());
    s->endGroup();
}

//-------------------------------------------------------------------------------------------------
void PlayerWidget::restore(QSettings *s)
{
    s->beginGroup("PlayerWidget");
    if (s->contains("Geometry"))
        setGeometry(s->value("Geometry").toRect());
    s->endGroup();
}

//-------------------------------------------------------------------------------------------------
void PlayerWidget::frameRendered(int i, QImage &img)
{
    ui->lblFrame->setText(QString("#%1").arg(i));
    ui->lblView->setPixmap(QPixmap::fromImage(img));
}
