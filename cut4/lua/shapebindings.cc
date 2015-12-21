#include "lua.hpp"

#include "lua/shapebindings.h"
#include "lua/luascene.h"
#include "render/c4scene.h"
#include "shape/c4shape.h"

namespace LuaShape {

    //---------------------------------------------------------------------------------------------
    int shapeCreateByMedia(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 1) {
            lua_pushstring(L, "shapeCreateByMedia: incorrect argument count ('shapeCreateByMedia(mediaName)' expected)");
            lua_error(L);
        }

        QString mediaName  = lua_tostring(L, 1);

        C4Shape *shape = luaScene->createMediaShape(mediaName);

        if (shape) {
            lua_pushinteger(L,shape->id());
            return 1;
        }
        return 0;
    }

    //---------------------------------------------------------------------------------------------
    int shapeSetPos(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 4) {
            lua_pushstring(L, "shapeSetPos: incorrect argument count ('shapeSetPos(id,x,y,z)' expected)");
            lua_error(L);
        }

        int   shapeId = lua_tointeger(L,1);
        float p[]     = { lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4) };

        C4Shape *shape = luaScene->shapeById(shapeId);
        if (!shape) {
            lua_pushstring(L, "shapeSetPos: incorrect shape id");
            lua_error(L);
        }
        shape->setPos(p);

        return 0;
    }

    //---------------------------------------------------------------------------------------------
    int shapeRotate(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 4) {
            lua_pushstring(L, "shapeRotate: incorrect argument count ('shapeRotate(rx,ry,rz)' expected)");
            lua_error(L);
        }

        int   shapeId = lua_tointeger(L,1);
        float r[]     = { lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4) };

        C4Shape *shape = luaScene->shapeById(shapeId);
        if (!shape) {
            lua_pushstring(L, "shapeRotate: incorrect shape id");
            lua_error(L);
        }
        shape->rotate(r);

        return 0;

    }

    //---------------------------------------------------------------------------------------------
    int shapeScale(lua_State *L)
    {
        Q_ASSERT(luaScene);

        int n = lua_gettop(L);    /* number of arguments */
        if (n != 4) {
            lua_pushstring(L, "shapeScale: incorrect argument count ('shapeScale(dx,dy,dz)' expected)");
            lua_error(L);
        }

        int   shapeId = lua_tointeger(L,1);
        float d[]     = { lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4) };

        C4Shape *shape = luaScene->shapeById(shapeId);
        if (!shape) {
            lua_pushstring(L, "shapeScale: incorrect shape id");
            lua_error(L);
        }
        shape->scale(d);

        return 0;
    }



}
