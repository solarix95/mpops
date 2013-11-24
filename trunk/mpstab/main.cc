#include <iostream>
#include <QApplication>
#include <QFuture>
#include <QtConcurrentRun>
#include <QImage>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QPair>
#include <QPainter>
#include <QFileDialog>
#include <QImageReader>
#include <math.h>
#include "shared/geometrypicker.h"
#include "himage.h"


#define VERSION "0.1.0"


//---------------------------------------------------------------
static void s_print_help()
{
  std::cout << std::endl;
  std::cout << "mpstab - motion picture stabilization - " << VERSION    << std::endl;
  std::cout << "usage: mpstab geometry inTemplate outTemplate"          << std::endl;

  std::cout << std::endl;
  std::cout << "examples:"                                                  << std::endl;
  std::cout << "  mpstab 100x100+160+90 in/frame_%d.png out/frame_%d.jpg"   << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;
}

//---------------------------------------------------------------
static bool s_parse_geometry(const QString &value, QRect &rect) // "widthxheight+x+y", "1024x768+10+20"
{
  if (value=="pick") {
      rect.setWidth(-1); // pick geometry later by dialog..
      return true;
  }

  QList<QString> position = value.split("+");

  if ((position.count() == 3) || (position.count() == 1)) { // with or without x/y-position
    QList<QString> dimension = position.takeFirst().split("x");
    if (dimension.count() == 2) {
      rect = QRect();
      if (position.count() == 2) {       // Position is optional..
        rect.setX(position[0].toInt());
        rect.setY(position[1].toInt());
      }

      rect.setWidth(dimension[0].toInt());
      rect.setHeight(dimension[1].toInt());
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------
typedef struct JobData_
{
  QString         imgName;
  QImage          img;
  int                              frameIndex;
  float                            distance;
  QFuture<QPair<QPoint,double> >   poi;
} JobData;

#define TO_STDSTRING(x)  x.toAscii().data()

inline double calcRgbDistance(QRgb c1, QRgb c2)
{
  return fabs(qRed  (c1) - qRed  (c2)) +
         fabs(qGreen(c1) - qGreen(c2)) +
         fabs(qBlue (c1) - qBlue (c2));
}

//---------------------------------------------------------------
QPoint nextPoint(const QPoint &p)
{
  static QList<QPoint> queue;
  queue << p;
  while (queue.size() > 1)
    queue.takeFirst();
  QPoint ret;
  foreach(QPoint np, queue) {
    ret.setX(ret.x() + np.x());
    ret.setY(ret.y() + np.y());
  }
  ret.setX(ret.x()/queue.count());
  ret.setY(ret.y()/queue.count());
  std::cout << "MAP Result: " << p.x() << "/" << p.y() << " -> " << ret.x() << "/" << ret.y()<< std::endl;
  return ret;
}


//---------------------------------------------------------------
QPair<QPoint,double> lookup(const HImage &ioi, const QImage &img, QPoint startPos, int tolerance)
{
  double minDistance = -1;
  QPair<QPoint,double> poi = qMakePair(QPoint(-1,-1),-1.0);

  if (tolerance >= img.width()/3)
    return poi;

  QPoint sp;
  sp.setY(startPos.y() - tolerance/2);
  if (sp.y() < 0)
    sp.setY(0);

  sp.setX(startPos.x() - tolerance/2);
  if (sp.x() < 0)
    sp.setX(0);

  QPoint ep;
  ep.setY(startPos.y() + tolerance/2);
  if (ep.y() > img.height()-ioi.height())
    ep.setY(img.height()-ioi.height());

  ep.setX(startPos.x() + tolerance/2);
  if (ep.x() > img.width()-ioi.width())
    ep.setX(img.width()-ioi.width());


  double dstep  = qMin(log(ioi.width()),log(ioi.height()));
  int step  = dstep < 1 ? 1 : (int)dstep;

  for (int oy=sp.y(); oy<ep.y(); oy++) {
    for (int ox=sp.x(); ox<ep.x(); ox++) {
      double distance   = 0;
      int steps = 0;
      for (int iy=0; iy<ioi.height()-step; iy += step) {
        for (int ix=0; ix<ioi.width()-step; ix += step) {
          float  innerHue   = ioi.hueAt(ix,iy);
          QColor outerColor = img.pixel(ox+ix,oy+iy);
          distance += fabs(innerHue - outerColor.hueF());
          steps++;
        }
      }
      distance = distance/3.0/steps;
      if ((minDistance < 0) || (distance < minDistance)) {
        minDistance = distance;
        poi.first = QPoint(ox,oy);
      }
    }
  }


  poi.second = minDistance;
  qDebug() << minDistance << step << tolerance << poi;

  if (minDistance > 0.1) {
    qDebug() << "DOUBLE AT" << minDistance;
    return lookup(ioi,img,startPos,tolerance*2);
  }
  return poi;
}

//---------------------------------------------------------------
int main(int argc, char *argv[])
{
  QApplication app(argc,argv);

  if (app.arguments().count() < 4) {
    s_print_help();
    return 1;
  }


  QRect   roi;
  if (!s_parse_geometry(app.arguments()[1],roi)) {
    s_print_help();
    return 1;
  }



  QString inTemplate  = app.arguments()[2];
  QString outTemplate = app.arguments()[3];

  QStringList fileList;

  if (inTemplate != "pick") {
      int frameIndex   = 1;
      do {
          QString nextFilename = QString().sprintf(inTemplate.toAscii().data(),frameIndex++);
          if (QFile::exists(nextFilename))
              fileList << nextFilename;
          else
              break;
      } while (true);
  } else {
      QString imageExtensions;
      foreach(QByteArray nextFormat, QImageReader::supportedImageFormats())
          imageExtensions += QString(" *.%1").arg(QString(nextFormat).toLower());
      fileList = QFileDialog::getOpenFileNames(NULL,"Select Timelapse Images","",QString("Images (%1)").arg(imageExtensions));
  }
  QList<JobData> results;
  int frameIndex   = 0;

  HImage  ioi;
  QImage  lookupImage;
  bool withRecalibrate = true;

  if ((roi.width() <=0) && (fileList.count() > 0) && (QFile::exists(fileList.first()))) { // must/can pick ROI?
      QImage img(fileList.first());
      if (img.isNull())
          return 1;
      GeometryPicker picker(&img,"ROI");
      if (!picker.exec())
          return 1;
      roi = picker.selection();
  }

  while (true) {

    JobData next;
    frameIndex++;
    next.imgName = fileList.count() ? fileList.takeFirst() : QString();
    if (next.imgName.isEmpty() || !QFile::exists(next.imgName)) {
      std::cout << "abort at: " << TO_STDSTRING(next.imgName)  << std::endl;
      break;
    }

    if (ioi.width() <= 0) {
      QImage imgioi;
      imgioi.load(next.imgName);
      lookupImage = imgioi.copy(roi);
      ioi = HImage(lookupImage);
    }

    if (next.img.load(next.imgName)) {
      std::cout << "init: " << TO_STDSTRING(next.imgName)  << std::endl;
      next.poi     = QtConcurrent::run(lookup, ioi, next.img,roi.topLeft(),next.img.width()/10);
      next.frameIndex = frameIndex;
      results << next;
    } else {
      std::cout << "abort at: " << TO_STDSTRING(next.imgName)  << std::endl;
      break;
    }

    if (results.count() > QThread::idealThreadCount()) {
      for (int r=0; r<results.count(); r++) {
        results[r].poi.waitForFinished();
        std::cout << TO_STDSTRING(results[r].imgName) << ": "
                  << results[r].poi.result().first.x() << "/"
                  << results[r].poi.result().first.y() << std::endl;

        QPoint poi = results[r].poi.result().first;

        poi = nextPoint(poi.x() > 0 ? poi:roi.topLeft());
        QImage out(results[r].img.width(),results[r].img.height(),QImage::Format_RGB32);
        out.fill(0);
        QPainter painter(&out);
        int dx = roi.topLeft().x()-poi.x();
        int dy = roi.topLeft().y()-poi.y();
        painter.drawImage(dx,dy,results[r].img);

        /*
        qDebug() << roi << poi << dx << dy;
        painter.setPen(QColor(Qt::blue));
        painter.drawRect(roi);
        painter.setPen(QColor(Qt::white));
        painter.drawRect(QRect(poi.x(),poi.y(),roi.width(),roi.height()));
        painter.drawText(50,50,QString("%1").arg(results[r].poi.result().second));

        painter.drawRect(200,50,lookupImage.width(),lookupImage.height());
        painter.drawImage(200,50,lookupImage);

        painter.drawRect(200+lookupImage.width()+10,50,lookupImage.width(),lookupImage.height());
        painter.drawImage(200+lookupImage.width()+10,50,results[r].img.copy(poi.x(),poi.y(),roi.width(),roi.height()));

        */

        out.save(QString().sprintf(outTemplate.toAscii().data(),results[r].frameIndex),NULL,99);
      }
      if (withRecalibrate && results.last().poi.result().first.x() > 0) {
        // int indexOfNewPoi = -1;
        // float
        std::cout << "Recalibrate" << std::endl;
        QRect newRoi(results.last().poi.result().first,roi.size());
        lookupImage = results.last().img.copy(newRoi);
        ioi = HImage(lookupImage);
      }
      results.clear();
    }
  }

  for (int r=0; r<results.count(); r++) {
    results[r].poi.waitForFinished();
    std::cout << TO_STDSTRING(results[r].imgName) << ": "
              << results[r].poi.result().first.x() << "/"
              << results[r].poi.result().first.y() << std::endl;
  }

  return 0;
}
