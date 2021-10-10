#ifndef SBIG_DEVICE_INFO_H
#define SBIG_DEVICE_INFO_H

#include <map>
#include <sbigudrv.h>
#include <string>

/// Class containing information on the SBIG Device.
class SbigSTDeviceInfo {

protected:
  /// Type of SBIG device. Can be USB, Parallel, or Ethernet.
  SBIG_DEVICE_TYPE device_type_ = DEV_NONE;
  /// Name for this device.
  std::string device_name_      = "NOT_INITIALIZED";
  /// The device's serial n umber.
  std::string serial_number_    = "NOT_INITIALIZED";
  /// Parallel port address for this device (if any)
  ushort lpt_base_address_      = 0;
  /// IP address for this device (if any)
  ulong ip_address_             = 0;
  /// Identifier for the filter wheel attached to this device.
  std::string filter_wheel_id_  = "";
  /// Map containing filters within the filter wheel.
  std::map<size_t, std::string> filters_;

public:
  /// Default constructor. Will have invalid device information.
  SbigSTDeviceInfo();

  /// Constructor for USB devices.
  /// \param name Name/Model for the device
  /// \param serial_number serial number for the device
  /// \param device_type Type of device
  /// \param filter_wheel_id [optional] Name/model of the filter wheel
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type,
                 std::string filter_wheel_id = "");

  /// Constructor for parallel port devices.
  /// \param name Name/Model for the device
  /// \param serial_number serial number for the device
  /// \param device_type Type of device
  /// \param lptBaseAddress LPT port address for the device.
  /// \param filter_wheel_id [optional] Name/model of the filter wheel
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type, ushort lptBaseAddress,
                 std::string filter_wheel_id = "");

  /// Constructor for network devices.
  /// \param name Name/Model for the device
  /// \param serial_number serial number for the device
  /// \param device_type Type of device
  /// \param ip_address IP address for the device.
  /// \param filter_wheel_id [optional] Name/model of the filter wheel
  SbigSTDeviceInfo(std::string name, std::string serial_number,
                 SBIG_DEVICE_TYPE device_type, ulong ip_address,
                 std::string filter_wheel_id = "");

  /// Destructor.
  ~SbigSTDeviceInfo();

  /// Overload for == operator.
  bool operator==(const SbigSTDeviceInfo &a) const;

public:
  /// Get a string describing this device.
  std::string ToString() const;

  /// Get the SBIG device type for this device.
  SBIG_DEVICE_TYPE GetDeviceType() const;

  /// Get the IP address for this device.
  ulong GetIPAddress() const;

  /// Get the LPT address for this device.
  ushort GetLPTAddress() const;

  /// Get this device's name.
  std::string GetDeviceName() const;

  /// Returns true if this device is valid and properly initialized.
  bool IsValidDevice();

  /// Get the name/model of the filter wheel.
  std::string GetFilterWheelID() const;

  /// Set the filter wheel ID
  void SetFilterWheelID(const std::string & filter_wheel_id);

  /// Configures the photometric filters by slot ID and name.
  void SetFilter(size_t slot_id, std::string filter_name);

  /// Get the photometric filters by ID and name.
  std::map<size_t, std::string> GetFilters();

  //
}; // class SbigSTDeviceInfo

#endif // SBIG_DEVICE_INFO_H
