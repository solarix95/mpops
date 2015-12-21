#include <QFileDialog>
#include "scriptwidget.h"
#include "ui_scriptform.h"
#include "render/c4scene.h"


//-------------------------------------------------------------------------------------------------
ScriptWidget::ScriptWidget(C4Scene *scene, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptWidget),
    mScene(scene)
{
    ui->setupUi(this);
    connect(ui->btnExec, SIGNAL(clicked()), this, SLOT(runScript()));
    connect(ui->btnLoad, SIGNAL(clicked()), this, SLOT(openScript()));
    connect(scene,SIGNAL(preRender(int)), this, SLOT(preRender(int)));
}

//-------------------------------------------------------------------------------------------------
ScriptWidget::~ScriptWidget()
{
    delete ui;
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::store(QSettings *s)
{
    s->beginGroup("ScriptWidget");
    s->setValue("Geometry",geometry());
    s->setValue("LastSource",mLastSource);
    s->endGroup();
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::restore(QSettings *s)
{
    s->beginGroup("ScriptWidget");
    if (s->contains("Geometry"))
        setGeometry(s->value("Geometry").toRect());
    mLastSource = s->value("LastSource").toString();
    s->endGroup();
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::runScript()
{
    switch(ui->tabRender->currentIndex()) {
    case 0: runInit(); break;
    case 1: runRender(); break;
    }
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::openScript()
{
    mLastSource = QFileDialog::getOpenFileName(this,"Load Lua",mLastSource,"*.lua");
    if (!mLastSource.isEmpty()) {
        switch(ui->tabRender->currentIndex()) {
        case 0: openInitScript(mLastSource); break;
        case 1: openRenderScript(mLastSource); break;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::preRender(int frame)
{
    mLua.setGlobal(LuaEngine::LuaFrame,frame);
    QString error = mLua.exec(ui->edtRender->toPlainText());
    ui->edtError->setText(error);
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::runInit()
{
    mLua.setGlobal(LuaEngine::LuaFrame, -1);
    QString error = mLua.exec(ui->edtInit->toPlainText());
    ui->edtError->setText(error);
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::runRender(int frame)
{
    if (frame < 0)
        frame = ui->spbFrame->value();
    /*
    mLua.setGlobal(LuaEngine::LuaFrame,frame);
    QString error = mLua.exec(ui->edtRender->toPlainText());
    ui->edtError->setText(error);
    if (error.isEmpty())
    */
    mScene->singleShot(NULL,frame);
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::openInitScript(const QString &fileName)
{
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    ui->edtInit->setText(f.readAll());
}

//-------------------------------------------------------------------------------------------------
void ScriptWidget::openRenderScript(const QString &fileName)
{
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    ui->edtRender->setText(f.readAll());
}
