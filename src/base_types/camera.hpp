#ifndef CAMERA_H
#define CAMERA_H

// local includes
#include "device.hpp"

// external includes
#include "camera-enums.pb.h"
#include "temperature-enums.pb.h"
#include "common.pb.h"

/// class
class Camera : public Device {

public:
  // constructor
  Camera();
  // destructor
  virtual ~Camera();

protected:
  //
  // Camera information, to be populated / modified by subclasses.
  //

  /// Vector containing the camera's capabilities. Should be cleared and populated
  /// by a subclass.
  std::vector<niad::CameraCapability> mCapabilities = {niad::CAMERA_CAPABILITY_NONE};

  /// Indicates the underlying sensor technology.
  niad::CameraSensorType mSensorType = niad::CAMERA_SENSOR_TYPE_UNKONWN;

  /// Indicates the color capabilities of the sensor.
  niad::CameraSensorColorType mColorType = niad::CAMERA_SENSOR_COLOR_UNKNOWN;

  /// Indicates the types of readout modes supported by the sensor.
  std::vector<niad::CameraReadoutMode> mReadoutModes = {niad::CAMERA_READOUT_MODE_UNKNOWN};

  /// Indicates the shutter capabilities
  std::vector<niad::CameraShutterAction> mShutterActions = {niad::CAMERA_SHUTTER_ACTION_NONE};

  /// Indicates supported camera image storage options.
  std::vector<niad::CameraImageAction> mImageActions = {niad::CAMERA_IMAGE_ACTION_NONE};

  /// Indicates the minimum allowable exposure duration (in seconds)
  double mExposureDurationMin = 0;

  /// Indicates the maximum allowable exposure duration (in seconds)
  double mExposureDurationMax = 1;

  /// Indicates the minimum allowable gain (arbitrary units)
  double mGainMin = 1;

  /// Indicates the maximum allowable gain (arbitrary units)
  double mGainMax = 1;

  /// Bit depth for the camera's detector
  niad::BitDepth mBitDepth;

  /// Number of pixels in the detector in (x,y,z) order.
  std::vector<int64_t> mPixelCount;

  /// Size of pixels in the detector in (x,y,z) order. Units: micrometers.
  std::vector<double> mPixelSize;

public:
  /// Get the camera's capabilities.
  virtual std::vector<niad::CameraCapability> getCapabilities();

  /// Get the number of pixels.
  /// \return vector in (x,y,z) order.
  virtual std::vector<int64_t> getPixelCount();

  /// Gets the sizes of pixels in micrometers.
  /// \return vector in (x,y,z) order
  virtual std::vector<double> getPixelSize();

  /// Gets the exposure duration minimum and maximum
  /// \return vector in (min, max) order.
  virtual std::vector<double> getExposureMinMax();

  /// Gets the exposure gain minimum and maximum
  /// \return vector in (min, max) order.
  virtual std::vector<double> getGainMinMax();

  /// Gets the type of sensor for this camera.
  virtual niad::CameraSensorType getSensorType();

  /// Gets the bit depth for the sensor
  virtual niad::BitDepth getBitDepth();

  /// Get the color type for the camera
  virtual niad::CameraSensorColorType getSensorColorType();

  /// Get the sensor readout modes
  virtual std::vector<niad::CameraReadoutMode> getReadoutModes();


public:

  /// Turn on/off active cooling
  /// \param sensor The sensor to target with the temperature setting
  /// \param set_active Set to true to enable cooling, false to disable cooling
  /// \param temperature The temperature set point (Celsius)
  virtual void setTemperatureTarget(niad::TemperatureType sensor,
                            bool set_active,
                            double temperature) = 0;

  /// Get the current temperature for the designated sensor
  /// \param sensor The sensor to inquire.
  virtual double getTemperature(niad::TemperatureType sensor) = 0;



  //
}; // Camera


#endif // CAMERA_H
