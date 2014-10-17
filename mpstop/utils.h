#ifndef UTILS_H
#define UTILS_H

#include <QList>
#include <QMimeData>
#include <QByteArray>

namespace Darksuit
{
QList<QByteArray>  filesFromMimeData(const QMimeData *mimeData);
}

#endif // UTILS_H
