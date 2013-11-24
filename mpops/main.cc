#include <iostream>
#include <stdlib.h>
#include <QApplication>
#include <QRect>
#include <QList>
#include <QString>
#include <QThread>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QImageReader>
#include "controller.h"
#include "shared/geometrypicker.h"

#define VERSION "0.1.0"

//---------------------------------------------------------------
static void s_print_help()
{
  std::cout << std::endl;
  std::cout << "mpops - motion picture operations - " << VERSION    << std::endl;
  std::cout << "usage: mpops [options] files..."                     << std::endl;
  std::cout << "  --crop-from=geometry"                              << std::endl;
  std::cout << "  --crop-to=geometry"                                << std::endl;
  std::cout << "  --resize=size       : widthxheight"                << std::endl;
  std::cout << "  --out=subdir"                                      << std::endl;
  std::cout << "  --format=ext"                                      << std::endl;
  std::cout << "  --tweening=strategy"                               << std::endl;
  std::cout << "  --rename=filename-template (sprintf)"              << std::endl;
  std::cout << "  --threads=count     : default: cpu-count)"         << std::endl;
  std::cout << "  --create-toc        : creates cinelerra-toc"       << std::endl;
  std::cout << std::endl;
  std::cout << "supported formats: "                                 << std::endl;

  foreach(QByteArray format, QImageReader::supportedImageFormats())
    std::cout << " " << format.data() << std::endl;

  std::cout << std::endl;
  std::cout << "supported tweening-strategies: "                   << std::endl;
  std::cout << " " << "avg            : average"                   << std::endl;

  std::cout << std::endl;
  std::cout << "examples:"                                            << std::endl;
  std::cout << "  mpops --resize=800x600 *.jpg"                       << std::endl;
  std::cout << "  mpops --resize=800x600 *.jpg --rename=frame_%d.jpg" << std::endl;
  std::cout << "  mpops --resize=800x600 img_1.jpg img_2.jpg"         << std::endl;
  std::cout << "  mpops --resize=800x600 img_%d.jpg"                  << std::endl;
  std::cout << "  mpops --format=png --out=converted *.jpg"           << std::endl;
  std::cout << "  mpops --crop-from=800x600+10+10 --crop-to=80x60 --resize=80x60 *.jpg" << std::endl;

  std::cout << std::endl;
  std::cout << std::endl;

}

//---------------------------------------------------------------
static bool s_parse_geometry(const QString &value, QRect &rect) // "widthxheight+x+y", "1024x768+10+20"
{
    if (value == "pick") {
        rect.setWidth(-1);
        rect.setHeight(-1);
        return true;
    } else {
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
    }
    return false;
}

//---------------------------------------------------------------
static bool s_parse_arg(const QString &arg, const QString &key, QString &value)
{
  if (arg.startsWith(QString("--%1=").arg(key))) {
    QList<QString> parts = arg.split("=",QString::SkipEmptyParts);
    if (parts.count() == 2) {
      value = parts[1];
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------
static void s_validate_args(const Args &args)
{
  if (args.fileList.isEmpty()) {
    std::cout << std::endl << "ERROR: empty file list" << std::endl;
    exit(-1);
  }
  if (args.threadCount < 1) {
    std::cout << std::endl << "ERROR: invalid thread-count" << std::endl;
    exit(-1);
  }

  if (!args.format.isEmpty()) {
    bool formatIsValid = false;
    foreach(QByteArray format, QImageReader::supportedImageFormats()) {
      if (format == args.format.toAscii())
        formatIsValid = true;
    }
    if (!formatIsValid) {
      std::cout << std::endl << "ERROR: unknown image-format" << std::endl;
      exit(-1);
    }
  }

  if (args.withToc) {
    if (!args.withResize) {
      std::cout << std::endl << "ERROR: TOC only with resize-option!" << std::endl;
      exit(-1);
    }
    if (!args.outfileTemplate.isEmpty() &&  !args.outfileTemplate.endsWith(".png")) {
      std::cout << std::endl << "ERROR: TOC only in 'png'-format (--rename=<template>.png)" << std::endl;
      exit(-1);
    }
    if (args.outfileTemplate.isEmpty() && (args.format != "png")) {
      std::cout << std::endl << "ERROR: TOC only with '--format=png' option!" << std::endl;
      exit(-1);
    }
  }

  if (!args.outfileTemplate.isEmpty() && !args.format.isEmpty()) {
    std::cout << std::endl << "ERROR: '--format' cannot be used with '--rename'!" << std::endl;
    exit(-1);
  }

  if (!args.tweening.isEmpty() && (args.tweening != "avg")) {
    std::cout << std::endl << "ERROR: invalid tweening strategy" << std::endl;
    exit(-1);
  }

  if (!args.tweening.isEmpty() && args.outfileTemplate.isEmpty()) {
    std::cout << std::endl << "ERROR:  tweening only with '--rename'" << std::endl;
    exit(-1);
  }
}

//---------------------------------------------------------------
static void s_parse_args(Args &args)
{
  QStringList argList = qApp->arguments();
  argList.takeFirst();

  foreach(QString nextArg, argList) {
    QString argValue;
    if (s_parse_arg(nextArg,"resize",argValue)) {                // --resize=1024x768
      QList<QString> parts = argValue.split("x");
      if (parts.count() == 2) {
        args.withResize = true;
        args.toSize     = QSize(parts[0].toInt(),parts[1].toInt());
      }
    } else if (s_parse_arg(nextArg,"crop-from",argValue) && s_parse_geometry(argValue,args.fromRect)) {  // --crop-from=1024x768+10+70
      args.withCropFrom = true;
    } else if (s_parse_arg(nextArg,"crop-to",argValue) && s_parse_geometry(argValue,args.toRect)) {  // --crop-to=1024x768+10+70
      args.withCropTo = true;
    } else if (s_parse_arg(nextArg,"threads",argValue)) {
      args.threadCount = argValue.toInt();
    } else if (s_parse_arg(nextArg,"out",argValue)) {
      args.outDir = argValue;
    } else if (s_parse_arg(nextArg,"format",argValue)) {
      args.format = argValue; 
    } else if (s_parse_arg(nextArg,"tweening",argValue)) {
      args.tweening = argValue;
    }else if (s_parse_arg(nextArg,"rename",argValue)) {
      args.outfileTemplate = argValue;
    }else if (s_parse_arg(nextArg,"add-alpha",argValue)) {
      QList<QString> parts = argValue.split(",");
      if (parts.count() == 2) {
        args.withAlpha = true;
        args.alphaHue  = parts[0].toInt();
        args.alphaHueTolerance = parts[1].toInt();
      }
    } else if (s_parse_arg(nextArg,"color-picker",argValue)) {
      QList<QString> parts = argValue.split("+");
      if (parts.count() == 2) {
        args.colorPickerPixel = QPoint(parts[0].toInt(),parts[1].toInt());
      }
    } else if (nextArg == "--create-toc") {
      args.withToc = true;
    } else if ((nextArg == "--help") || (nextArg == "-h") || (nextArg == "-?")) {
      args.printHelp = true;
    } else {
      args.fileList << nextArg;
    }
  }
}

//---------------------------------------------------------------
int main(int argc, char *argv[])
{
  QApplication app(argc,argv);

  Args args;
  s_parse_args(args);

  if (args.printHelp) {
    s_print_help();
    return -1;
  }

  s_validate_args(args);
  if (!args.outDir.isEmpty()) {
    QDir dir;
    if (!dir.mkpath(args.outDir)) {
      std::cout << std::endl << "ERROR: cannot create outdir " << std::endl;
      return -1;
    }
  }

  Controller c(args);
  QObject::connect(&c, SIGNAL(finished()), &app, SLOT(quit()));

  if (c.run())
    return app.exec();
  return 0;
}
