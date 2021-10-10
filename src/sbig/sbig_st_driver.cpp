
// local includes
#include "sbig_st_driver.hpp"
#include "sbig_st_errors.hpp"
#include "sbig_st_device_info.hpp"
#include "sbig_st_device.hpp"

// project includes
#include "utilities.hpp"

// system includes
#include <sstream>
#include <algorithm>
#include <thread>

SbigSTDriver::SbigSTDriver()
  : do_readout_(false) {
  Open();
}

SbigSTDriver::~SbigSTDriver() {
  Close();
}

std::vector<std::string> SbigSTDriver::GetSupportedCameras() {
  std::vector<std::string> output;
  output.push_back("ST-7");
  output.push_back("ST-8");
  output.push_back("ST-9");
  output.push_back("ST-10");
  output.push_back("ST-5C");
  output.push_back("ST-237");
  output.push_back("ST-K");
  output.push_back("ST-V");
  output.push_back("ST-1K");
  output.push_back("ST-2K");
  output.push_back("ST-L");
  output.push_back("ST-402");
  output.push_back("ST-X");
  output.push_back("ST-4K");
  output.push_back("ST-T");
  output.push_back("ST-I");
  output.push_back("ST-F");
  return output;
}

std::vector<std::string> SbigSTDriver::GetSupportedFilterWheels() {
  return SbigSTFilterWheel::GetSupportedFilterWheels();
};

void SbigSTDriver::RunCommand(short command, void *params, void *results) {

  // get exclusive control of the driver, then issue the command.
  const std::lock_guard<std::mutex> lock(driver_access_mutex_);
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(command, params, results));
}

void SbigSTDriver::RunCommand(short command, void *params, void *results, short handle) {

  // get exclusive control of the driver
  const std::lock_guard<std::mutex> lock(driver_access_mutex_);

  // Switching device handles is an exceptionally expensive operation for the
  // SBIG driver. Do so only if necessary.
  if(handle != active_device_handle_) {
    SetDriverHandleParams handle_p;
    handle_p.handle = handle;
    SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_SET_DRIVER_HANDLE, &handle_p, nullptr));
    active_device_handle_ = handle;
  }

  // run the command
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(command, params, results));
}

void SbigSTDriver::Open() {
  // open the driver
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_OPEN_DRIVER, nullptr, nullptr));
}

void SbigSTDriver::Close() {

  // Explicitly close all devices
  SetDriverHandleParams handle_p;
  for(auto it = active_devices_.begin(); it != active_devices_.end(); it++) {
    CloseDevice(it->second);
  }

  // Explicitly close the driver, ignoring errors as those would cause the application
  // to terminate.
  SBIGUnivDrvCommand(CC_CLOSE_DRIVER, nullptr, nullptr);
}

std::string SbigSTDriver::ToString() {

  std::stringstream ss;

  auto info = GetInfo();
  ss << "Driver Name: " << info.name << "\n"
     << "Version: " << info.version << "\n"
     << "Max Requests: " << info.maxRequests;

  return ss.str();
}

SbigSTDriverInfo SbigSTDriver::GetInfo() {

  // run the query
  GetDriverInfoParams info_p;
  info_p.request = 0;
  GetDriverInfoResults0 info_r;
  RunCommand(CC_GET_DRIVER_INFO, &info_p, &info_r);

  // construct and return the result
  SbigSTDriverInfo info;
  info.version = std::to_string(bcd2float(info_r.version));
  info.name = info_r.name;
  info.maxRequests = info_r.maxRequest;
  return info;
}

std::vector<SbigSTDeviceInfo> SbigSTDriver::GetDeviceList() {

  std::vector<SbigSTDeviceInfo> list;

  // Query for USB devices first. Create the corresponding SbigSTDevices.
  QueryUSBResults2 usb_r;
  RunCommand(CC_QUERY_USB2, nullptr, &usb_r);

  for(int i = 0; i < usb_r.camerasFound; i++) {
    auto info = usb_r.usbInfo[i];

    // Set the USB port information.
    SBIG_DEVICE_TYPE type = DEV_USB;
    switch(i) {
    case 0: type = DEV_USB1; break;
    case 1: type = DEV_USB2; break;
    case 2: type = DEV_USB3; break;
    case 3: type = DEV_USB4; break;
    case 4: type = DEV_USB5; break;
    case 5: type = DEV_USB6; break;
    case 6: type = DEV_USB7; break;
    case 7: type = DEV_USB8; break;
    }

    // Create the device object. Append it to the list.
    SbigSTDeviceInfo d(info.name, info.serialNumber, type);
    list.push_back(d);
  }

  // TODO: In the future add Ethernet and Parallel devices here.

  return list;
}

std::shared_ptr<SbigSTDevice> SbigSTDriver::OpenDevice(const SbigSTDeviceInfo & info) {

  // get exclusive control over the driver
  std::unique_lock<std::mutex> lock(driver_access_mutex_);

  // Check to see if the device is already open. If so, return that device.
  for(auto it = active_devices_.begin(); it != active_devices_.end(); ++it) {
    auto h = it->first;
    auto d = it->second;
    auto d_info = d->GetInfo();

    if(info == d_info) {
      return active_devices_[h];
    }
  }

  OpenDeviceParams dev_params;
  dev_params.deviceType     = info.GetDeviceType();
  dev_params.ipAddress      = info.GetIPAddress();
  dev_params.lptBaseAddress = info.GetLPTAddress();

  // open the device
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_OPEN_DEVICE, &dev_params, nullptr));

  // Get the device handle
  GetDriverHandleResults handle_r;
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_GET_DRIVER_HANDLE, nullptr, &handle_r));

  // Switch to this device handle.
  SetDriverHandleParams handle_p;
  handle_p.handle = handle_r.handle;
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_SET_DRIVER_HANDLE, &handle_r, nullptr));
  active_device_handle_ = handle_p.handle;

  // Establish a link.
  EstablishLinkParams link_p;
  EstablishLinkResults link_r;
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_ESTABLISH_LINK, &link_p, &link_r));

  // Make the device, add it to the map, return it to the caller.
  auto device = std::make_shared<SbigSTDevice>(info, handle_r.handle);
  active_devices_[handle_r.handle] = device;

  // TODO: Rework the initialization sequence for devices so we needn't use
  // non RAII conventions here:
  // Release the lock as all driver operations are complete.
  lock.unlock();

  // Initialize the device before returning it to the user.
  device->InitializeDevice();
  return device;
}

void SbigSTDriver::CloseDevice(std::shared_ptr<SbigSTDevice> device) {

  // get exclusive control over the driver
  const std::lock_guard<std::mutex> lock(driver_access_mutex_);

  // Remove the device from the list of active devices.
  for(auto it = active_devices_.begin(); it!= active_devices_.end(); ++it) {
    if(it->second == device) {
      active_devices_.erase(it);
      break;
    }
  }

  // Close the device with the driver.
  short handle = device->GetHandle();
  SetDriverHandleParams handle_p;
  handle_p.handle = handle;
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_SET_DRIVER_HANDLE, &handle_p, nullptr));
  SBIG_CHECK_STATUS(SBIGUnivDrvCommand(CC_CLOSE_DEVICE, &handle_p, nullptr));

  // Device is closed, remove it from the list of active devices
  //  active_devices_.erase(handle);
}


ImageData * SbigSTDriver::DoReadout(short device_handle,
                            short detector_id,
                            uint16_t bin_mode,
                            uint16_t top,
                            uint16_t left,
                            uint16_t width,
                            uint16_t height) {

  // allocate the image output buffer
  ImageData * img = new ImageData(width, height);

  // Obtain exclusive access for the driver to do a readout.
  std::lock_guard<std::mutex> readout_lock(device_readout_mutex_);

  // freeze the cooler
  SetTemperatureRegulationParams2 temp_reg_p;
  temp_reg_p.regulation = 3;
  RunCommand(CC_SET_TEMPERATURE_REGULATION2, &temp_reg_p, nullptr,
             device_handle);

  // indicate the readout should proceed
  do_readout_ = true;

  // start the readout
  StartReadoutParams sr_p;
  sr_p.ccd = detector_id;
  sr_p.readoutMode = bin_mode;
  sr_p.top = top;
  sr_p.left = left;
  sr_p.height = height;
  sr_p.width = width;
  RunCommand(CC_START_READOUT, &sr_p, nullptr, device_handle);

  // read out lines
  ReadoutLineParams rl_p;
  rl_p.ccd = detector_id;
  rl_p.readoutMode = bin_mode;
  rl_p.pixelStart = left;
  rl_p.pixelLength = width;
  for (size_t i = 0; (i < height) && do_readout_; i++) {
    auto pTmp = img->data.data() + (i * width); // pointer math
    RunCommand(CC_READOUT_LINE, &rl_p, pTmp, device_handle);
  }

  // end the readout
  EndReadoutParams er_p;
  er_p.ccd = 0;
  RunCommand(CC_END_READOUT, &er_p, nullptr, device_handle);

  // indicate the readout should not proceed
  do_readout_ = false;

  // un-freeze the cooler
  temp_reg_p.regulation = 5;
  RunCommand(CC_SET_TEMPERATURE_REGULATION2, &temp_reg_p, nullptr,
             device_handle);

  return img;
}

void SbigSTDriver::AbortReadout() {
  do_readout_ = false;
}

SbigSTDeviceInfo SbigSTDriver::FindDevice(std::string device_id,
                          std::string filter_wheel_id) {

  SbigSTDeviceInfo output;

  // Verify that the device_id is supported
  auto dev_ids = GetSupportedCameras();
  if(std::find(dev_ids.begin(), dev_ids.end(), device_id) == dev_ids.end()) {
    std::cout << "Device ID " << device_id
              << " is not supported by this driver."
              << std::endl;
    return output;
  }

  // Verify that the filter_wheel_id is supported
  auto fw_ids = GetSupportedFilterWheels();
  if(filter_wheel_id != "" &&
     std::find(fw_ids.begin(), fw_ids.end(), filter_wheel_id) == fw_ids.end()) {
    std::cout << "Filter Wheel ID " << filter_wheel_id
              << " is not supported by this driver." << std::endl;
    return output;
  }

  // Attempt to find the device_id in the list of active devices.
  auto dev_infos = GetDeviceList();
  for(auto info: dev_infos) {
    if (info.GetDeviceName().find(device_id) != std::string::npos) {
      // SBIG Cameras don't really auto-detect their filter wheel, so here
      // we simply assign the filter_wheel_id to help the device later.
      info.SetFilterWheelID(filter_wheel_id);
      return info;
    }
  }

  return output;
}
