#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QString>

struct lua_State;
class C4Scene;


class LuaEngine : public QObject
{
    Q_OBJECT
public:
    enum GlobalSymbol {
        LuaFrame,
        LuaTime
    };

    explicit LuaEngine(QObject *parent = 0);
    virtual ~LuaEngine();
    
    void    setGlobal(GlobalSymbol, const QVariant &v);
    QString exec(const QString &script);

    static void setScene(C4Scene *sScene);

signals:
    
private:
    typedef QMap<QString, QVariant> LuaGlobals;

    lua_State      *mLua;
    LuaGlobals      mGlobals;

};

#endif // LUAENGINE_H
