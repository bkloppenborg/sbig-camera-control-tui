#include "sbig_st_device_info.hpp"

SbigSTDeviceInfo::SbigSTDeviceInfo() {
  // default constructor which contains invalid device information
}

SbigSTDeviceInfo::SbigSTDeviceInfo(std::string name, std::string serial_number,
                               SBIG_DEVICE_TYPE device_type,
                               std::string filter_wheel_id) {
  device_type_ = device_type;
  device_name_ = name;
  serial_number_ = serial_number;
  filter_wheel_id_ = filter_wheel_id;
}

SbigSTDeviceInfo::SbigSTDeviceInfo(std::string name, std::string serial_number,
                               SBIG_DEVICE_TYPE device_type, ushort lpt_base_address,
                               std::string filter_wheel_id) {
  device_type_ = device_type;
  device_name_ = name;
  serial_number_ = serial_number;
  lpt_base_address_ = lpt_base_address;
  filter_wheel_id_ = filter_wheel_id;
}

SbigSTDeviceInfo::SbigSTDeviceInfo(std::string name, std::string serial_number,
                               SBIG_DEVICE_TYPE device_type, ulong ip_address,
                               std::string filter_wheel_id) {
  device_type_ = device_type;
  device_name_ = name;
  serial_number_ = serial_number;
  ip_address_ = ip_address;
  filter_wheel_id_ = filter_wheel_id;
}

SbigSTDeviceInfo::~SbigSTDeviceInfo() {

}

bool SbigSTDeviceInfo::operator==(const SbigSTDeviceInfo &a) const {
  if(a.device_type_ == device_type_ &&
     a.device_name_ == device_name_ &&
     a.serial_number_ == serial_number_&&
     a.lpt_base_address_ == lpt_base_address_ &&
     a.ip_address_ == ip_address_ &&
     a.filter_wheel_id_ == filter_wheel_id_)
    return true;

  return false;
}

std::string SbigSTDeviceInfo::ToString() const {
  return device_name_;
}

SBIG_DEVICE_TYPE SbigSTDeviceInfo::GetDeviceType() const {
  return device_type_;
}

ulong SbigSTDeviceInfo::GetIPAddress() const {
  return ip_address_;
}

ushort SbigSTDeviceInfo::GetLPTAddress() const {
  return lpt_base_address_;
}
std::string SbigSTDeviceInfo::GetDeviceName() const {
  return device_name_;
}
bool SbigSTDeviceInfo::IsValidDevice() {
  if(device_type_ != DEV_NONE)
    return true;

  return false;
}

std::string SbigSTDeviceInfo::GetFilterWheelID() const {
  return filter_wheel_id_;
}

void SbigSTDeviceInfo::SetFilterWheelID(const std::string &filter_wheel_id) {
  filter_wheel_id_ = filter_wheel_id;
}

void SbigSTDeviceInfo::SetFilter(size_t slot_id, std::string filter_name) {
  filters_[slot_id] = filter_name;
}

std::map<size_t, std::string> SbigSTDeviceInfo::GetFilters() {
  return filters_;
}
