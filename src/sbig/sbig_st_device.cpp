#include "sbig_st_device.hpp"
#include "sbig_st_driver.hpp"
#include "sbig_st_device_info.hpp"

#include <sstream>
#include <iostream>
#include <exception>

SbigSTDevice::SbigSTDevice(SbigSTDeviceInfo info, short handle)
  : info_(info), device_handle_(handle) {

  // TODO: Verify device is registered with the driver. We can't let the user
  // create devices on their own.
}

SbigSTDevice::~SbigSTDevice() {
}

std::string SbigSTDevice::ToString() {
  return info_.ToString();
}

void SbigSTDevice::InitializeDevice() {

  // Query to get device capabilities
  auto device_name = info_.GetDeviceName();
  if (device_name.find("ST-") != std::string::npos) {
    InitializeSTSeries();
  } else if (device_name.find("PixCel") != std::string::npos) {
    InitializePixCelSeries();
  } else {
    // We don't know about this device type. Leave uninitialized.
    throw std::logic_error("Unknown camera type. Cannot initialize.");
  }

}
void SbigSTDevice::InitializeSTSeries() {

  // Get access to the SBIG driver
  SbigSTDriver &drv = SbigSTDriver::GetInstance();

  GetCCDInfoParams info_p;

  //
  // Setup primary imaging detector
  //
  // Query for basic detector information.
  info_p.request = 0; // Query 0 = Standard request for imaging detector
  GetCCDInfoResults0 imaging_info_r0;
  drv.RunCommand(CC_GET_CCD_INFO, &info_p, &imaging_info_r0, device_handle_);
  // Query for advanced detector information.
  info_p.request = 4; // Query 4 = secondary extended request for imaging CCD
  GetCCDInfoResults4 imaging_info_r4;
  drv.RunCommand(CC_GET_CCD_INFO, &info_p, &imaging_info_r4, device_handle_);
  // Initialize the main camera with relevant data.
  main_camera_ = std::make_shared<SbigSTCamera>(this, device_handle_, 0,
                                                imaging_info_r0,
                                                imaging_info_r4);

  //
  // Setup tracking detector
  //
  // Query for basic detector information
  info_p.request = 1; // Query 1 = Standard request for tracking detector
  GetCCDInfoResults0 tracking_info_r0;
  drv.RunCommand(CC_GET_CCD_INFO, &info_p, &tracking_info_r0, device_handle_);
  // Query for advanced detector information
  info_p.request = 5;
  GetCCDInfoResults4 tracking_info_r4;
  drv.RunCommand(CC_GET_CCD_INFO, &info_p, &tracking_info_r4, device_handle_);
  // configure the guide camera
  if (imaging_info_r0.firmwareVersion > 0) {
    guide_camera_ = std::make_shared<SbigSTCamera>(this, device_handle_, 1,
                                                   tracking_info_r0,
                                                   tracking_info_r4);
  }

  //
  // Setup filter wheel
  //
  if (info_.GetFilterWheelID() != "") {
    has_filter_wheel_ = true;
    filter_wheel_ = std::make_shared<SbigSTFilterWheel>(this,
                                                        info_.GetFilterWheelID(),
                                                        info_.GetFilters());
  }
}

void SbigSTDevice::InitializePixCelSeries() {

}

void SbigSTDevice::SetTemperatureRegulation(bool turn_on, double temperature) {

  // Get access to the SBIG driver
  SbigSTDriver &drv = SbigSTDriver::GetInstance();

  SetTemperatureRegulationParams2 temp_p;
  if(turn_on)
    temp_p.regulation = 1;
  else
    temp_p.regulation = 0; // regulation off

  temp_p.ccdSetpoint = temperature;

  drv.RunCommand(CC_SET_TEMPERATURE_REGULATION2, &temp_p, nullptr, device_handle_);

}

TemperatureInfo SbigSTDevice::GetTemperatureInfo() {

  // Get access to the SBIG driver
  SbigSTDriver &drv = SbigSTDriver::GetInstance();

  // Set up and execute the query.
  QueryTemperatureStatusParams temp_p;
  temp_p.request = TEMP_STATUS_ADVANCED2;
  QueryTemperatureStatusResults2 temp_r;
  drv.RunCommand(CC_QUERY_TEMPERATURE_STATUS, &temp_p, &temp_r, device_handle_);

  // Construct the output structure and return it.
  TemperatureInfo ti;
  ti.fan_on                               = temp_r.fanEnabled;
  ti.temperature_regulation_on            = temp_r.coolingEnabled;
  ti.main_camera_temperature_setpoint     = temp_r.ccdSetpoint;
  ti.main_camera_temperature              = temp_r.imagingCCDTemperature;
  ti.tracking_camera_temperature_setpoint = temp_r.trackingCCDSetpoint;
  ti.tracking_camera_temperature          = temp_r.trackingCCDSetpoint;
  ti.ambient_temperature                  = temp_r.ambientTemperature;
  ti.heatsink_temperature                 = temp_r.heatsinkTemperature;
  ti.fan_percent_power                    = temp_r.fanPower;
  ti.fan_speed                            = temp_r.fanSpeed;

  return ti;
}

bool SbigSTDevice::ImageInProgress() {
  bool image_in_progress = false;
  image_in_progress |= main_camera_->ImageInProgress();
  image_in_progress |= guide_camera_->ImageInProgress();
  return image_in_progress;
}
