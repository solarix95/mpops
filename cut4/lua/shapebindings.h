#ifndef SHAPEBINDINGS_H
#define SHAPEBINDINGS_H

struct lua_State;

namespace LuaShape {
    int shapeCreateByMedia(lua_State *L);
    int shapeSetPos(lua_State *L);
    int shapeRotate(lua_State *L);
    int shapeScale(lua_State *L);

}
#endif // SHAPEBINDINGS_H
