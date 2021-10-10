#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

// vendor-specific includes
#include "sbig_st_camera.hpp"
#include "sbig_st_device.hpp"
#include "sbig_st_driver.hpp"

// local includes
#include "client.hpp"

// External includes
#include "niad.pb.h"

// system includes
#include <QObject>
#include <QDateTime>
#include <QDir>
#include <QWebSocket>
#include <memory>
#include <QWebSocket>

class Worker : public QObject {
  Q_OBJECT;

public:
  Worker(Client * client);
  ~Worker();

protected:
  Client * mClient = nullptr;

protected:
  std::shared_ptr<Camera> mMainCamera;
  std::shared_ptr<Camera> mGuideCamera;
  std::shared_ptr<FilterWheel> mFilterWheel;
  niad::CameraReadoutMode mReadoutMode = niad::CAMERA_READOUT_MODE_1X1;

  QString mObjectName = "NO_NAME";
  int mExposureQuanity = 0;
  double mExposureDuration = 0.0;

  QString mFilterName = "";

  QDir mSaveDir = QDir("/tmp/");

  QDateTime mStartTime;
  QDateTime mStopTime;


public slots:
  void run();

signals:
  void send(const niad::Envelope & e);
  void finished();

protected:
  int setupCamera();

public:
  void setTemperature(double temperature);

  void setFilter(const QString & filter_name);
  void setFilter(int filter_id);
  void setReadoutMode(niad::CameraReadoutMode mode);

  void setObjectName(const QString & object_name);
  void setExposureQuantity(int quantity);
  void setExposureDuration(double duration);

  void setStartDateTime(QDateTime & dt);
  void setStopDateTime(QDateTime & dt);

  void setSaveDir(const QString & directory);

      //
  }; // Worker

#endif // CAMERA_CONTROLLER_H
