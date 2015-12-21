#ifndef SCENEBINDINGS_H
#define SCENEBINDINGS_H

struct lua_State;

namespace LuaScene {
    int sceneInit(lua_State *L);
    int sceneSetAttribute(lua_State *L);
}

#endif // SCENEBINDINGS_H
