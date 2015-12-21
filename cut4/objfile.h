#ifndef OBJFILE_H
#define OBJFILE_H

#include <QString>
class ObjFile
{
public:
    ObjFile();

    bool load(const QString &filename);

    int   verticeCount() const;
    float *vertice(int index) const;

private:
    QList<float*> mVertices;

};

#endif // OBJFILE_H
