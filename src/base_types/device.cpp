// local includes
#include "device.hpp"

// system includes
#include <string>
#include <functional>

Device::Device() {
  // Create an ID for this device
  std::hash<size_t> hasher;
  mID = hasher((size_t) this);
}

Device::~Device() {
}

std::string Device::getName() { return mName; }
int Device::getVersionMajor() { return mVersion.major; };
int Device::getVersionMinor() { return mVersion.minor; };
int Device::getVersionPatch() { return mVersion.patch; };

DeviceID_t Device::getId()    { return mID; }

VersionNumber Device::getVersionNumber() { return mVersion; }

niad::DeviceConnectionType Device::getDeviceConnectionType() {
  return mDeviceConnectionType;
}

niad::DeviceType Device::getDeviceType() { return mDeviceType; }


