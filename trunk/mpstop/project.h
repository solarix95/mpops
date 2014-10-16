#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include "shared/cinelerratoc.h"

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(QObject *parent = 0);
    
    bool open(const QString &tocfile);
    void save();
    void saveAs(const QString &tocfile);
    bool isDirty() const;

signals:
    void clearAllFrames();
    void appendFrames(const QStringList &filenames);
    void appendFreeze();
    
private:
    CinelerraToc  mTocFile;
    bool          mIsDirty;
};

#endif // PROJECT_H
