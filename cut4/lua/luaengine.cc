#include <QDebug>
#include "lua/luaengine.h"
#include "lua/luascene.h"
#include "lua/scenebindings.h"
#include "lua/shapebindings.h"
#include "lua.hpp"
#include "render/c4scene.h"

//-------------------------------------------------------------------------------------------------
LuaEngine::LuaEngine(QObject *parent) :
    QObject(parent)
{
    mLua = luaL_newstate();
    luaL_openlibs(mLua);

    lua_register(mLua,"sceneInit"         ,&LuaScene::sceneInit);
    lua_register(mLua,"sceneSetAttribute" ,&LuaScene::sceneSetAttribute);
    lua_register(mLua,"shapeCreateByMedia",&LuaShape::shapeCreateByMedia);
    lua_register(mLua,"shapeSetPos",&LuaShape::shapeSetPos);
    lua_register(mLua,"shapeRotate",&LuaShape::shapeRotate);
    lua_register(mLua,"shapeScale",&LuaShape::shapeScale);
}

//-------------------------------------------------------------------------------------------------
LuaEngine::~LuaEngine()
{
    lua_close(mLua);
}

//-------------------------------------------------------------------------------------------------
void LuaEngine::setGlobal(LuaEngine::GlobalSymbol s, const QVariant &v)
{
    switch (s) {
      case LuaFrame : mGlobals["_f"] = v;   break;
      case LuaTime  : mGlobals["_t"] = v;   break;
    }
}

//-------------------------------------------------------------------------------------------------
QString LuaEngine::exec(const QString &script)
{

    LuaGlobals::const_iterator it;
    for (it = mGlobals.begin(); it != mGlobals.end(); ++it) {
        QString  name = it.key();
        QVariant value = it.value();
        bool pushed = false;
        switch (value.type()) {
        case QVariant::Int : {
            lua_pushinteger(mLua,value.toInt());
            pushed = true;
        } break;
        case QVariant::Double : {
            lua_pushnumber(mLua,value.toDouble());
            pushed = true;
        } break;
        case QVariant::String : {
            lua_pushstring(mLua,value.toString().toUtf8().constData());
            pushed = true;
        } break;
        case QVariant::Bool : {
            lua_pushboolean(mLua,value.toBool());
            pushed = true;
        } break;
        default:;
        }
        if (pushed)
            lua_setglobal(mLua,name.toUtf8());
        else
            qWarning() << "LuaEngine::exec: invalid global variable type" << value.type();
    }

    QString ret;
    int retCode = luaL_dostring(mLua,script.toUtf8());
    if (retCode)
        ret = lua_tostring(mLua, -1);
    return ret;
}

//-------------------------------------------------------------------------------------------------
void LuaEngine::setScene(C4Scene *scene)
{
    Q_ASSERT(!luaScene);
    luaScene = scene;
    Q_ASSERT(luaScene);
}
