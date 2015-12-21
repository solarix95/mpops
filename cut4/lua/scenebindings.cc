#include "lua/scenebindings.h"

#include "lua.hpp"
#include "lua/luascene.h"
#include "render/c4scene.h"

namespace LuaScene {

    //---------------------------------------------------------------------------------------------
    int sceneInit(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 2) {
            lua_pushstring(L, "sceneInit: incorrect argument count ('sceneInit(width, height)' expected)");
            lua_error(L);
        }

        int width  = lua_tonumber(L, 1);
        int height = lua_tonumber(L, 2);

        luaScene->init(QSize(width,height));


        return 0;
    }

    //---------------------------------------------------------------------------------------------
    int sceneSetAttribute(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 2) {
            lua_pushstring(L, "sceneSetAttribute: incorrect argument count ('sceneSetAttribute(attr, value)' expected)");
            lua_error(L);
        }

        QString aName  = lua_tostring(L, 1);
        float   aValue  = lua_tonumber(L, 2);

        luaScene->setAttribut(C4Scene::StereoEffect,aValue);

        return 0;
    }
}
