#ifndef SBIG_DEVICE_INFO_H
#define SBIG_DEVICE_INFO_H

#include <map>
#include <sbigudrv.h>
#include <string>

class SbigSTDeviceInfo {

protected:
  SBIG_DEVICE_TYPE device_type_ = DEV_NONE;
  std::string device_name_      = "NOT_INITIALIZED";
  std::string serial_number_    = "NOT_INITIALIZED";
  ushort lpt_base_address_      = 0;
  ulong ip_address_             = 0;
  std::string filter_wheel_id_  = "";
  std::map<size_t, std::string> filters_;

public:
  SbigSTDeviceInfo(); // invalid device information.
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type,
                 std::string filter_wheel_id = "");
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type, ushort lptBaseAddress,
                 std::string filter_wheel_id = "");
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type, ulong ip_address,
                 std::string filter_wheel_id = "");
  ~SbigSTDeviceInfo();

  bool operator==(const SbigSTDeviceInfo &a) const;

public:
  std::string ToString() const;
  SBIG_DEVICE_TYPE GetDeviceType() const;
  ulong GetIPAddress() const;
  ushort GetLPTAddress() const;
  std::string GetDeviceName() const;

  bool IsValidDevice();

  std::string GetFilterWheelID() const;
  void SetFilterWheelID(const std::string & filter_wheel_id);
  void SetFilter(size_t slot_id, std::string filter_name);
  std::map<size_t, std::string> GetFilters();

  //
}; // class SbigSTDeviceInfo

#endif // SBIG_DEVICE_INFO_H
