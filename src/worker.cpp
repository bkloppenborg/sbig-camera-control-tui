#include "worker.hpp"

// system includes
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

#include "datetime_utilities.hpp"
#include <google/protobuf/util/time_util.h>

Worker::Worker(Client * client)
  : QObject(nullptr), mClient(client), mStopExposures(false) {
}

Worker::~Worker() {
}

void Worker::run() {

  int status = setupCamera();
  if(status != 0) {
    qDebug() << "Camera initialization failed. Bailing...";
    emit finished();
    return;
  }

  qInfo() << "Worker ready at "
          << QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

  if(mSetTemperature) {
    mMainCamera->setTemperatureTarget(niad::TEMPERATURE_TYPE_SENSOR, true, mTemperatureTarget);
  }

  // Check to see if the filter is a number
  bool filter_name_is_number = false;
  int filter_id = mFilterName.toInt(&filter_name_is_number);
  if (filter_name_is_number) {
    mFilterWheel->setFilter(filter_id);
  } else {
    mFilterWheel->setFilter(mFilterName.toStdString());
  }

  for (size_t exp_num = 0; exp_num < mExposureQuanity; exp_num++) {

    if(mStopExposures)
      break;

    qDebug() << "Starting exposure" << exp_num;

    // Instruct the client to buffer positions
    mClient->startBuffering();

    // Take the image.
    ImageData *image_data =
      mMainCamera->acquireImage(mExposureDuration, mReadoutMode, mShutterAction);

    // Instruct the client to stop buffering.
    mClient->stopBuffering();

    // Find the closest values that are applicable. Add them to the image.
    auto coordinates = mClient->getCoordinates();
    if(coordinates.size() > 0) {
      size_t midpoint = coordinates.size() / 2;
      auto & c = coordinates[midpoint];
      if(c.type() == niad::COORDINATE_TYPE_RA_DEC) {
        image_data->ra = c.position(0);
        image_data->dec = c.position(1);
        image_data->ra_dec_set = true;
      } else if (c.type() == niad::COORDINATE_TYPE_AZM_ALT) {
        image_data->azm = c.position(0);
        image_data->alt = c.position(1);
        image_data->azm_alt_set = true;
      }
    }

    auto mount_lla = mClient->getLLA();
    if(mount_lla.position_size() > 0) {
      image_data->latitude  = mount_lla.position(0);
      image_data->longitude = mount_lla.position(1);
      image_data->altitude  = mount_lla.position(2);
    }

    // Populate the image with some additional information.
    image_data->object_name = mObjectName.toStdString();

    QString filename = QDateTime::currentDateTimeUtc().toString(Qt::ISODate) +
      "_" + mObjectName + ".fits";
    filename = mSaveDir.filePath(filename);
    image_data->saveToFITS(filename.toStdString(), true);
    qDebug() << "Saved " << filename;

    // Delete the image data
    delete image_data;
  }

  emit finished();
}

int Worker::setupCamera() {

  using namespace std;

  // Set up the SBIG camera.
  string c_name = "ST-10";
  string f_name = "CFW-8";
  auto &driver = SbigSTDriver::GetInstance();
  // find a device matching those specifications
  auto info = driver.FindDevice(c_name, f_name);
  if (!info.IsValidDevice()) {
    qInfo() << "Failed to find the specified device.";
    return -1;
  }

  string filter_set = "RGBL";
  if (filter_set == "RGBL") {
    info.SetFilter(1, "Red");
    info.SetFilter(2, "Green");
    info.SetFilter(3, "Blue");
    info.SetFilter(4, "Clear");
    info.SetFilter(5, "None");
  } else if (filter_set == "UBVRI") {
    info.SetFilter(1, "U");
    info.SetFilter(2, "B");
    info.SetFilter(3, "V");
    info.SetFilter(4, "R");
    info.SetFilter(5, "I");
  }

  // Get a shared pointer to the cameras and filter wheel.
  auto device = driver.OpenDevice(info);
  mMainCamera = device->GetMainCamera();
  mGuideCamera = device->GetGuideCamera();
  mFilterWheel = device->GetFilterWheel();

  // Verify that we found the cameras.
  if (mMainCamera == nullptr || mGuideCamera == nullptr) {
    qCritical() << "Could not find specified camera.";
    return -1;
  }

  // Verify that we found the filter wheel.
  if (mFilterWheel == nullptr) {
    qCritical() << "Could not find specified filter wheel.";
    return -1;
  }

  // Print out information on the discovered device.
  qInfo() << "Found " << QString::fromStdString(c_name)
          << " with " << QString::fromStdString(f_name);

  // Print out information on the filters
  QString filter_info;
  filter_info.append("Filter Set: ");
  for(auto f: mFilterWheel->getFilterNames()) {
    filter_info.append(QString::fromStdString(f) + ",");
  }
  qInfo() << filter_info;

  return 0;
}

void Worker::setTemperature(double temperature)  {

  bool enable_cooler = false;
  if(temperature < 40)
    enable_cooler = true;

  mTemperatureTarget = temperature;
  mSetTemperature = true;
}

void Worker::setFilter(const QString &filter_name) {
  mFilterName = filter_name;
}

void Worker::setReadoutMode(niad::CameraReadoutMode mode) {
  mReadoutMode = mode;
}

void Worker::setObjectName(const QString &object_name) {
  mObjectName = object_name;
}

void Worker::setExposureQuantity(int quantity) {
  mExposureQuanity = quantity;
}

void Worker::setExposureDuration(double duration) {
  mExposureDuration = duration;
}

void Worker::setSaveDir(const QString &directory) {
  mSaveDir = directory;
}

void Worker::stopExposures() {
  mStopExposures = true;

  mMainCamera->abortExposure();
}

void Worker::setShutterAction(niad::CameraShutterAction action) {
  mShutterAction = action;
}
