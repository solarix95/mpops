#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include "videosource.h"
#include "videobuffer.h"

namespace Ui {
class MainView;
}

class MainView : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainView(QWidget *parent = 0);
    virtual ~MainView();
    
private slots:
    void sourceReady();

private:
    Ui::MainView *ui;

    VideoSource *mSource;
    VideoBuffer *mBuffer;
    QString      mSourceName;
};

#endif // MAINVIEW_H
