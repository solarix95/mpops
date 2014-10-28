#ifndef SELECTIONMODEL_H
#define SELECTIONMODEL_H

#include <QObject>
#include <QSet>
#include <QList>

class SelectionModel : public QObject
{
    Q_OBJECT

public:
    SelectionModel();

    inline bool isSelected(int index) const { return mIndexes.contains(index); }
    inline QList<int> selections() const { QList<int> ret = mIndexes; qSort(ret); return ret; }

signals:
    void changed(int index);
    void selected(int index);


public slots:
    void select(int index);
    void select(int index, bool deselectOthers);
    void arraySelect(int toIndex);
    void deselect(int index);
    void removed(int index);

private:
    QList<int> mIndexes;

};

#endif // SELECTIONMODEL_H
