#include "filter_wheel.hpp"

FilterWheel::FilterWheel() {
  
}

FilterWheel::~FilterWheel() {
  
}

std::vector<niad::FilterWheelCapability> FilterWheel::getCapabilities() {
  return mCapabilities;
}
