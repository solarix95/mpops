
#include <QApplication>
#include "mainview.h"
#include "resourcewidget.h"
#include "playerwidget.h"
#include "scriptwidget.h"
#include "render/c4scene.h"
#include "lua/luaengine.h"
#include <QSettings>

//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    C4Scene scene;
    LuaEngine::setScene(&scene);
    QSettings settings("HELD", "CUT4");

    //--------------------------------------------------------------------------------------------
    Mainview w(&scene);
    w.show();

    PlayerWidget player(&scene);
    player.restore(&settings);
    player.show();

    ScriptWidget scripts(&scene);
    scripts.restore(&settings);
    scripts.show();

    ResourceWidget resources(&scene);
    resources.restore(&settings);
    resources.show();

    //--------------------------------------------------------------------------------------------

    int ret = app.exec();

    player.store(&settings);
    scripts.store(&settings);
    resources.store(&settings);

    return ret;
}
