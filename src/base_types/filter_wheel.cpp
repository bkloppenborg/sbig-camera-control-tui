#include "filter_wheel.hpp"

FilterWheel::FilterWheel() {

}

FilterWheel::~FilterWheel() {

}

std::vector<niad::FilterWheelCapability> FilterWheel::getCapabilities() {
  return mCapabilities;
}

std::string FilterWheel::getActiveFilterName() {

  std::string name = "";

  try {
    name = mSlotToFilterName.at(mActiveSlot);
  } catch (...) {
    // do nothing
  }

  return name;
}

size_t FilterWheel::getActiveFilterSlot() {
  return mActiveSlot;
}

std::vector<std::string> FilterWheel::getFilterNames() {
  using namespace std;
  vector<string> filter_names;

  for (auto it: mSlotToFilterName) {
    filter_names.push_back(it.second);
  }

  return filter_names;
}

std::map<size_t, std::string> FilterWheel::getSlotToFilterMap() {
  return mSlotToFilterName;
}
