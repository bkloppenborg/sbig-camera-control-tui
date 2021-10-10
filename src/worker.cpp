#include "worker.hpp"

// system includes
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

#include "datetime_utilities.hpp"
#include <google/protobuf/util/time_util.h>

Worker::Worker(Client * client)
  : QObject(nullptr), mClient(client) {

  mStartTime = QDateTime::currentDateTimeUtc();
  mStopTime = QDateTime::currentDateTimeUtc();
  mStopTime = mStopTime.addDays(1);
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

  // Set the filter wheel. This is a blocking call.
  mFilterWheel->setFilter(mFilterName.toStdString());

  if (QDateTime::currentDateTimeUtc() < mStartTime) {
    qInfo() << "Waiting until specified time to start exposures...";
  }

  // Wait until the start time has elapsed, waking up every 100 ms.
  while (QDateTime::currentDateTimeUtc() < mStartTime) {
    QThread::msleep(100);
  }

  for (size_t exp_num = 0; exp_num < mExposureQuanity; exp_num++) {

    qDebug() << "Starting exposure" << exp_num;
    // Exit the loop if we have exceeded the stop date-time.
    auto now = QDateTime::currentDateTimeUtc();
    if (now.addSecs(mExposureDuration) > mStopTime) {
      qDebug() << "Stop time exceeded. Exiting.";
      break;
    }

    // Instruct the client to buffer positions
    mClient->startBuffering();

    // Take the image.
    ImageData *image_data =
      mMainCamera->acquireImage(mExposureDuration, mReadoutMode);

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

    QString filename = mObjectName + "_" +
      QDateTime::currentDateTimeUtc().toString(Qt::ISODate) +
      ".fits";
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

  // Set the temperature
  mMainCamera->setTemperatureTarget(niad::TEMPERATURE_TYPE_SENSOR,
                                    enable_cooler, temperature);

  qInfo() << "Set temperature to " << temperature << " C" << endl;
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

void Worker::setStartDateTime(QDateTime &dt) {
  mStartTime = dt;
}

void Worker::setStopDateTime(QDateTime &dt) {
  mStopTime = dt;
};

void Worker::setSaveDir(const QString &directory) {
  mSaveDir = directory;
}
