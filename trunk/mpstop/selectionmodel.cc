#include <QDebug>
#include "selectionmodel.h"

SelectionModel::SelectionModel()
{
}

void SelectionModel::select(int index, bool deselectOthers)
{
    while (deselectOthers && mIndexes.count() > 0)
        deselect(mIndexes.first());

    qDebug() << mIndexes << index << deselectOthers;
    if (!mIndexes.contains(index)) {
        mIndexes.append(index);
        emit changed(index);
        emit selected(index);
    }
}

void SelectionModel::deselect(int index)
{
    mIndexes.removeAll(index);
    emit changed(index);
}

void SelectionModel::removed(int index)
{
    mIndexes.removeAll(index);
    emit changed(index);
    QList<int>::Iterator i;
    for (i = mIndexes.begin(); i != mIndexes.end(); ++i) {
        if (*i > index) {
            *i = *i - 1;
            emit changed(*i);
        }
    }
}

void SelectionModel::select(int index)
{
    select(index,true);
}
