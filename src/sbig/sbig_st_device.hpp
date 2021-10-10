#ifndef SBIG_DEVICE_H
#define SBIG_DEVICE_H

#include "sbig_st_camera.hpp"
#include "sbig_st_filter_wheel.hpp"
#include "sbig_st_device_info.hpp"

#include <sbigudrv.h>
#include <string>
#include <memory>

/// Structure to store temperature information from the camera.
struct TemperatureInfo {
  /// True if the fan is on.
  bool   fan_on                               = false;
  /// True if temperature regulation is active.
  bool   temperature_regulation_on            = false;
  /// Temperature set point for the main camera (Celsius)
  double main_camera_temperature_setpoint     = 0.0;
  /// Temperature of the main camera (Celsius)
  double main_camera_temperature              = 0.0;
  /// Temperature set point for the tracking camera (Celsius)
  double tracking_camera_temperature_setpoint = 0.0;
  /// Temperature of the tracking camera (Celsius)
  double tracking_camera_temperature          = 0.0;
  /// Ambient temperature (Celsius)
  double ambient_temperature                  = 0.0;
  /// Heatsink temperature (Celsiu)
  double heatsink_temperature                 = 0.0;
  /// Percent power for the fan (%)
  double fan_percent_power                    = 0.0;
  /// Fan seep (RPM)
  double fan_speed                            = 0.0;


}; // struct TemperatureInfo

/// Class representing a SBIG ST device.
class SbigSTDevice {

private:

  SbigSTDeviceInfo info_; ///< Information about this device.
  short device_handle_   = -1; ///< Device handle for this device;
  bool is_open_          = false; ///< Whether or not this device is open.
  bool has_main_camera_  = false; ///< True if this device has a main camera.
  bool has_guide_camera_ = false; ///< True if this device has a guide camera.
  bool has_focus_motor_  = false; ///< True if this device has a focus motor.
  bool has_filter_wheel_ = false; ///< True if this device has a filter wheel.

  /// Shared pointer to the main camera (if any)
  std::shared_ptr<SbigSTCamera>    main_camera_  = nullptr;
  /// Shared pointer to the guide camera (if any)
  std::shared_ptr<SbigSTCamera>    guide_camera_ = nullptr;
  /// Shared pointer to the filter wheel (if any)
  std::shared_ptr<SbigSTFilterWheel> filter_wheel_ = nullptr;

public:
  /// Initialize this device.
  void InitializeDevice();
private:
  /// Initialization specific to ST-series cameras.
  void InitializeSTSeries();
  /// Initialization specific to PixCel series cameras.
  void InitializePixCelSeries();

public:
  /// Default constructor
  /// \param info Information to find this specific device.
  /// \param handle Device handle.
  SbigSTDevice(SbigSTDeviceInfo info, short handle);
  /// Default destructor.
  ~SbigSTDevice();

  /// Get the handle to this device.
  short GetHandle() { return device_handle_; }

  /// Get information about this device.
  const SbigSTDeviceInfo GetInfo() { return info_; }

  /// Get a string describing this device.
  std::string ToString();

  /// Determine if this device has a main camera.
  /// \return true if a main camera exists.
  bool HasMainCamera() { return has_main_camera_; }

  /// Get a shared pointer to the main camera.
  /// NOTE: If the underlying driver goes out of scope, access to the camera
  /// will fail.
  std::shared_ptr<SbigSTCamera> GetMainCamera() { return main_camera_; }

  /// Determine if this device has a guide camera.
  /// \return true if a main camera exists.
  bool HasGuideCamera() { return has_guide_camera_; };

  /// Get a shared pointer to the guide camera.
  /// NOTE: If the underlying driver goes out of scope, access to the camera
  /// will fail.
  std::shared_ptr<SbigSTCamera> GetGuideCamera() { return guide_camera_; }

  /// Determine if this device has a filter wheel camera.
  /// \return true if a main camera exists.
  bool HasFilteWheel() { return has_filter_wheel_; }

  /// Get a shared pointer to the filter wheel.
  /// NOTE: If the underlying driver goes out of scope, access to the filter
  /// wheel will fail.
  std::shared_ptr<SbigSTFilterWheel> GetFilterWheel() { return filter_wheel_; }

  /// Enable, disable, and set temperature regulation
  /// \param turn_on Set to true to enable temperature regulation, false to disable.
  /// \param temperature Temperature set point (Celsius)
  void SetTemperatureRegulation(bool turn_on, double temperature = 0.0);

  /// Get information about the camera's temperature.
  TemperatureInfo GetTemperatureInfo();

  /// Activate temperature regulation.
  bool TemperatureRegulationOn();

  /// Determine if an image is in progress.
  bool ImageInProgress();

  //
}; // SbigSTDevice


#endif // SBIG_DEVICE_H
