#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QSettings>

namespace Ui {
class PlayerForm;
}

class C4Scene;
class PlayerWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlayerWidget(C4Scene *scene, QWidget *parent = 0);
    ~PlayerWidget();
    
    void store(QSettings *s);
    void restore(QSettings *s);

private slots:
    void frameRendered(int,QImage&);

private:
    Ui::PlayerForm *ui;
    C4Scene        *mScene;
};

#endif // PLAYERWIDGET_H
