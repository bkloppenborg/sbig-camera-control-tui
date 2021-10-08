#ifndef SBIG_DEVICE_H
#define SBIG_DEVICE_H

#include "sbig_st_camera.hpp"
#include "sbig_st_filter_wheel.hpp"
#include "sbig_st_device_info.hpp"

#include <sbigudrv.h>
#include <string>
#include <memory>

struct TemperatureInfo {
  bool   fan_on = false;
  bool   temperature_regulation_on = false;
  double main_camera_temperature_setpoint     = 0.0;
  double main_camera_temperature              = 0.0;
  double tracking_camera_temperature_setpoint = 0.0;
  double tracking_camera_temperature          = 0.0;
  double ambient_temperature                  = 0.0;
  double heatsink_temperature                 = 0.0;
  double fan_percent_power                    = 0.0;
  double fan_speed                            = 0.0; // RPM


}; // struct TemperatureInfo

class SbigSTDevice {

private:

  SbigSTDeviceInfo info_;
  short device_handle_   = -1; // default no-device handle value;
  bool is_open_          = false;
  bool has_main_camera_  = false;
  bool has_guide_camera_ = false;
  bool has_focus_motor_  = false;
  bool has_filter_wheel_ = false;

  std::shared_ptr<SbigSTCamera>    main_camera_  = nullptr;
  std::shared_ptr<SbigSTCamera>    guide_camera_ = nullptr;
  std::shared_ptr<SbigSTFilterWheel> filter_wheel_ = nullptr;

public:
  void InitializeDevice();
private:
  void InitializeSTSeries();
  void InitializePixCelSeries();

public:
  SbigSTDevice(SbigSTDeviceInfo info, short handle);
  ~SbigSTDevice();

  short GetHandle() { return device_handle_; }

  const SbigSTDeviceInfo GetInfo() { return info_; }
  std::string ToString();

  bool HasMainCamera() { return has_main_camera_; }
  std::shared_ptr<SbigSTCamera> GetMainCamera() { return main_camera_; }

  bool HasGuideCamera() { return has_guide_camera_; };
  std::shared_ptr<SbigSTCamera> GetGuideCamera() { return guide_camera_; }

  bool HasFilteWheel() { return has_filter_wheel_; }
  std::shared_ptr<SbigSTFilterWheel> GetFilterWheel() { return filter_wheel_; }

  void SetTemperatureRegulation(bool turn_on, double temperature = 0.0);
  TemperatureInfo GetTemperatureInfo();
  bool TemperatureRegulationOn();

  bool ImageInProgress();

  //
}; // SbigSTDevice


#endif // SBIG_DEVICE_H
