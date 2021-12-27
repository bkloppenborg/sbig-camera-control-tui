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

/// Class to control the camera.
class Worker : public QObject {
  Q_OBJECT;

public:
  /// Default constructor
  /// \param client NIAD client, if any.
  Worker(Client * client);

  /// Default destructor.
  ~Worker();

  std::atomic<bool> mStopExposures;

protected:
  /// NIAD client object.
  Client * mClient = nullptr;

protected:
  /// Shared pointer to the main camera
  std::shared_ptr<Camera> mMainCamera;

  /// Shared pointer to the guild camera.
  std::shared_ptr<Camera> mGuideCamera;

  /// Shared pointer to the filter wheel.
  std::shared_ptr<FilterWheel> mFilterWheel;

  /// Camera readout mode. Defaults to 1x1.
  niad::CameraReadoutMode mReadoutMode = niad::CAMERA_READOUT_MODE_1X1;

  /// The quantity of exposures requested.
  int mExposureQuanity = 0;

  /// The duration of each exposure (in seconds)
  double mExposureDuration = 0.0;

  /// Name of the object being imaged.
  QString mObjectName = "NO_NAME";

  /// Name of the filter being used.
  QString mFilterName = "";

  /// Directory into which the images will be saved.
  QDir mSaveDir = QDir("/tmp/");

  bool mSetTemperature = false;
  double mTemperatureTarget = 0;

  niad::CameraShutterAction mShutterAction = niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE;

public slots:

  /// Slot to begin the thread.
  void run();

signals:
  /// Signal to indicate that all camera-related operations have completed.
  void finished();

protected:
  /// Connect to the specified camera(s) and filter wheel and initialize them.
  /// \return Non-zero value on any failure
  int setupCamera();

public:
  /// Specify the desired temperature for the camera. It will be set in run().
  /// \param temperature Set point temperature for the camera in Celsius. Any
  ///        value >= +40 C shuts off temperature regulation.
  void setTemperature(double temperature);

  /// Specify the desired photometric filter in the filter wheel.
  /// It will be set in run().
  /// \param filter_name The name of the filter.
  void setFilter(const QString & filter_name);

  /// Specify the desired photometric filter slot within the filter wheel.
  /// It will be set in run(). Prefer setFilter(QString) instead.
  /// \param filter_id The numeric ID for the desired filter slot.
  void setFilter(int filter_id);

  /// Specify the desired camera readout mode. It will be used within run().
  /// \param mode The desired readout mode for the camera.
  void setReadoutMode(niad::CameraReadoutMode mode);

  /// Set the name of the object being observed. This will be used within run()
  /// to specify a prefix on the save files AND encoded within the FITS file.
  /// \param object_name The name of the object. Keep below 68 characters.
  ///        and use underscores when possible.
  void setObjectName(const QString & object_name);

  /// Sets the quantity of exposures to be acquired during run()
  /// \param quantity Total number of exposures to be acquired.
  void setExposureQuantity(int quantity);

  /// Sets the duration of an individual exposure to be acquired during run().
  /// \param duration Duration, in seconds, for an exposure.
  void setExposureDuration(double duration);

  /// Set the time at which the exposures should begin.
  /// \param dt DateTime at which the exposures should begin.
  void setStartDateTime(QDateTime & dt);

  /// Sets the time at which the last exposure must be done.
  /// \param dt DateTime at which the last exposure should have completed.
  void setStopDateTime(QDateTime & dt);

  /// Sets the directory for save files.
  /// \param directory Directory into which files should be saved.
  void setSaveDir(const QString & directory);

  /// Indicate to the worker thread that exposures should stop.
  void stopExposures();

  /// Sets the action (if any) the shutter should take
  void setShutterAction(niad::CameraShutterAction action);

      //
  }; // Worker

#endif // CAMERA_CONTROLLER_H
