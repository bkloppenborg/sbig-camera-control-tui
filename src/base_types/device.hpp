#ifndef DEVICE_H
#define DEVICE_H

#include "version_number.hpp"

// external includes
#include "device-enums.pb.h"

// system includes
#include <string>

typedef uint32_t DeviceID_t;

/// Base class for devices
class Device {

public:
  Device();
  ~Device();

private:
  /// Unique identifier for this device. Set automatically in Device::Device()
  DeviceID_t mID = 0;

protected:
  /// Name of this mount. To be set by subclass in constructor.
  std::string mName = "DEVICE_NAME_UNKNOWN";

  /// Version information for the firmware or hardware for this device.
  /// To be set by subclass in constructor.
  VersionNumber mVersion;

protected:

  /// Indicates the physical connection type used by the Mount subclass.
  /// Used by main (or similar) to probe ports and/or let the user choose
  /// a device to connect to. Should be set in the subclass constructor.
  niad::DeviceConnectionType mDeviceConnectionType =
      niad::DEVICE_CONNECTION_TYPE_UNKNOWN;

  /// Indicates the type of device. Should be set in the subclass constructor.
  niad::DeviceType mDeviceType = niad::DEVICE_TYPE_UNKNOWN;

public:

  /// Get the identifier for this device
  DeviceID_t getId();

  /// Get the name for the device
  std::string getName();

  /// Sets the name for this device.
  void setName(std::string name);

  /// Get the version number of this device.
  VersionNumber getVersionNumber();

  /// Get the type of connection the device is using. Set by subclass constructor.
  niad::DeviceConnectionType getDeviceConnectionType();

  /// Get the type of device. Set by subclass constructor.
  niad::DeviceType getDeviceType();

  /// Get the device version major number.
  int getVersionMajor();
  /// Get the device version minor number.
  int getVersionMinor();
  /// Get the device version patch number.
  int getVersionPatch();

public:

  /// Called externally to initialize subclass variables.
  /// \return true if the device is fully initialized, false otherwise.
  virtual bool init() = 0;

  //
}; // class Device


#endif // DEVICE_H
