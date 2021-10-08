#include "sbig_st_filter_wheel.hpp"
#include "sbig_st_driver.hpp"
#include "sbig_st_device.hpp"

#include <algorithm>
#include <iostream>
#include <thread>

SbigSTFilterWheel::SbigSTFilterWheel(SbigSTDevice * device,
                                 std::string filter_wheel_id,
                                 std::map<size_t, std::string> slot_to_filter_name)
  : device_(device) {
  Setup(filter_wheel_id, slot_to_filter_name);
  Open();
}

SbigSTFilterWheel::~SbigSTFilterWheel() {
  Close();
}

void SbigSTFilterWheel::Setup(std::string filter_wheel_id,
                            std::map<size_t, std::string> slot_to_filter_name) {

  uint16_t max_slots = 0;
  if(filter_wheel_id == "CFW-2") {
    model_     = CFWSEL_CFW2;
    max_slots = 2;
  } else if(filter_wheel_id == "CFW-5") {
    model_ = CFWSEL_CFW5;
    max_slots = 4;
  } else if(filter_wheel_id == "CFW-8") {
    model_ = CFWSEL_CFW8;
    max_slots = 5;
  } else if(filter_wheel_id == "CFW-L") {
    model_ = CFWSEL_CFWL;
    max_slots = 5;
  } else if(filter_wheel_id == "CFW-402") {
    model_ = CFWSEL_CFW402;
    max_slots = 4;
  } else if(filter_wheel_id == "CFW-6A") {
    model_ = CFWSEL_CFW6A;
    max_slots = 6;
  } else if(filter_wheel_id == "CFW-10") {
    model_ = CFWSEL_CFW10;
    max_slots = 10;
  } else if(filter_wheel_id == "CFW-10-serial") {
    model_ = CFWSEL_CFW10_SERIAL;
    max_slots = 10;
  } else if(filter_wheel_id == "CFW-9") {
    model_ = CFWSEL_CFW9;
    max_slots = 5;
  } else if(filter_wheel_id == "CFW-L8") {
    model_ = CFWSEL_CFWL8;
    max_slots = 8;
  } else if(filter_wheel_id == "CFW-L8G") {
    model_ = CFWSEL_CFWL8G;
    max_slots = 8;
  } else if(filter_wheel_id == "CFW-1603") {
    model_ = CFWSEL_CFW1603;
    max_slots = 5;
  } else if(filter_wheel_id == "FW5-STX") {
    model_ = CFWSEL_FW5_STX;
    max_slots = 5;
  } else if (filter_wheel_id == "FW5-8300") {
    model_ = CFWSEL_FW5_8300;
    max_slots = 5;
  } else if (filter_wheel_id == "FW8-8300") {
    model_ = CFWSEL_FW8_8300;
    max_slots = 8;
  } else if (filter_wheel_id == "FW7-STX") {
    model_ = CFWSEL_FW7_STX;
    max_slots = 7;
  } else if (filter_wheel_id == "FW8-STT") {
    model_ = CFWSEL_FW8_STT;
    max_slots = 8;
  }

  // configure the information structure
  info_.max_slots = max_slots;
  info_.model_name = filter_wheel_id;
  info_.active_slot = 1;
  info_.slot_to_filter_name = slot_to_filter_name;
}

std::vector<std::string> SbigSTFilterWheel::GetSupportedFilterWheels() {
  std::vector<std::string> output;
  output.push_back("CFW-2");
  output.push_back("CFW-5");
  output.push_back("CFW-8");
  output.push_back("CFW-L");
  output.push_back("CFW-402");
  output.push_back("CFW-6A");
  output.push_back("CFW-10");
  output.push_back("CFW-10-serial");
  output.push_back("CFW-9");
  output.push_back("CFW-L8");
  output.push_back("CFW-L8G");
  output.push_back("CFW-1603");
  output.push_back("FW5-STX");
  output.push_back("FW5-8300");
  output.push_back("FW8-8300");
  output.push_back("FW7-STX");
  output.push_back("FW8-STT");

  return output;
};

void SbigSTFilterWheel::Open() {

  // get the driver
  auto &drv = SbigSTDriver::GetInstance();

  // open the port
  CFWParams cfw_p;
  CFWResults cfw_r;
  cfw_p.cfwModel = model_;
  cfw_p.cfwCommand = CFWC_OPEN_DEVICE;
  drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());

  // Query the filter to make sure it is really there
  cfw_p.cfwCommand = CFWC_QUERY;
  drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());
}

void SbigSTFilterWheel::Close() {

  // get the driver
  auto &drv = SbigSTDriver::GetInstance();

  // close the port to the filter wheel.
  CFWParams cfw_p;
  CFWResults cfw_r;
  cfw_p.cfwModel = model_;
  cfw_p.cfwCommand = CFWC_CLOSE_DEVICE;

  // The close command appears to fail
  try {
    drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());
  } catch (...) {
  }

}

void SbigSTFilterWheel::Init() {

  // get the driver
  auto &drv = SbigSTDriver::GetInstance();

  // instruct the filter wheel to re-initialize
  CFWParams cfw_p;
  CFWResults cfw_r;
  cfw_p.cfwModel = model_;
  cfw_p.cfwCommand = CFWC_INIT;
  drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());
}

double SbigSTFilterWheel::setFilter(size_t filter_slot) {

  // Do not move the filter wheel when an image is in progress
  if(device_->ImageInProgress())
    return 0;

  // get the driver
  auto &drv = SbigSTDriver::GetInstance();

  // instruct the filter wheel to re-initialize
  CFWParams cfw_p;
  CFWResults cfw_r;
  cfw_p.cfwModel = model_;
  cfw_p.cfwCommand = CFWC_GOTO;
  cfw_p.cfwParam1 = filter_slot;
  drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());

  // poll for the goto to complete
  bool goto_in_progress = true;
  cfw_p.cfwCommand = CFWC_QUERY;
  do {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    drv.RunCommand(CC_CFW, &cfw_p, &cfw_r, device_->GetHandle());
    if(cfw_r.cfwStatus == CFWS_IDLE || cfw_r.cfwError > 1)
      goto_in_progress = false;

  } while(goto_in_progress);

  info_.active_slot = filter_slot;

  return 0.0;
}

double SbigSTFilterWheel::setFilter(std::string filter_name) {

  // Find the filter in the slot-to-filter map. Goto the slot if found.
  for(auto const& it: info_.slot_to_filter_name) {
    if(it.second == filter_name)
      setFilter(it.first);
  }

  return 0;
}

std::string SbigSTFilterWheel::GetActiveFilterName() {

  std::string name = "";

  try {
    name = info_.slot_to_filter_name.at(info_.active_slot);
  } catch (...) {
    // do nothing
  }

  return name;
}

size_t SbigSTFilterWheel::GetActiveFilterSlot() {
  return info_.active_slot;
}
