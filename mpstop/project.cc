#include "project.h"

Project::Project(QObject *parent) :
    QObject(parent)
{
    mIsDirty = true;
}

bool Project::isDirty() const
{
    return mIsDirty;
}
