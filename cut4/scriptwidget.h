#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>
#include <QSettings>
#include "lua/luaengine.h"

namespace Ui {
class ScriptWidget;
}

class C4Scene;

//-------------------------------------------------------------------------------------------------
class ScriptWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ScriptWidget(C4Scene *scene, QWidget *parent = 0);
    ~ScriptWidget();
    
    void store(QSettings *s);
    void restore(QSettings *s);

public slots:
    void runRender(int frame = -1);

private slots:
    void runScript();
    void openScript();
    void preRender(int frame);


private:
    void runInit();

    void openInitScript(const QString &fileName);
    void openRenderScript(const QString &fileName);

    Ui::ScriptWidget *ui;
    C4Scene          *mScene;
    LuaEngine         mLua;
    QList<QString>    mSourceFiles;

    QString           mLastSource;
    QString           mInitScriptName;
    QString           mRenderScriptName;
};

#endif // SCRIPTWIDGET_H
