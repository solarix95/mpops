#include <QFile>
#include <QDebug>
#include <QStringList>
#include "objfile.h"

ObjFile::ObjFile()
{
}

bool ObjFile::load(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
            return false;
    while (!f.atEnd()) {
        QString line = f.readLine();
        QList<QString> parts = line.split(' ',QString::KeepEmptyParts);
        if (parts.count() == 4 && parts[0] == "v") {
            float *p = new float[3];
            p[0] = parts[1].toFloat();
            p[1] = parts[2].toFloat();
            p[2] = parts[3].toFloat();
            mVertices << p;
        }
    }
    return true;
}

int ObjFile::verticeCount() const
{
    return mVertices.count();
}

float *ObjFile::vertice(int index) const
{
    return mVertices[index];
}
