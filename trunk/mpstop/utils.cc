#include "utils.h"

#include <QFile>


namespace Darksuit
{

QList<QByteArray>  filesFromMimeData(const QMimeData *mimeData)
{
    Q_ASSERT(mimeData);
    QList<QByteArray> ret;

    // Getestet auf Ubuntu 12.04...
    QByteArray fileUrls = mimeData->data("text/uri-list");
    QList<QByteArray> fileUrl = fileUrls.split('\n');
    foreach(QByteArray nextFile, fileUrl) {
        nextFile = nextFile.trimmed();
        if (nextFile.isEmpty())
            continue;
        if (nextFile.startsWith("file://"))
            nextFile.remove(0,strlen("file://"));
        if (QFile::exists(nextFile)) {
           ret << nextFile;
        }
    }

    return ret;
}

}
