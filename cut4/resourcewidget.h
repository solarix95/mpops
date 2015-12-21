#ifndef RESOURCEWIDGET_H
#define RESOURCEWIDGET_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class ResourceWidget;
}

class C4Scene;
class ResourceWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ResourceWidget(C4Scene *scene, QWidget *parent = 0);
    ~ResourceWidget();
    
    void store(QSettings *s);
    void restore(QSettings *s);

private:
    Ui::ResourceWidget *ui;

    C4Scene            *mScene;
};

#endif // RESOURCEWIDGET_H
