#include "ExposureSettings.hpp"

#include <limits>

ExposureSettings::ExposureSettings(double duration_sec,
                                   ShutterAction shutter_action,
                                   READOUT_BINNING_MODE binning_mode){
  exposure_duration_sec_ = duration_sec;
  shutter_action_ = shutter_action;
  binning_mode_ = binning_mode;
  left_ = 0;
  top_ = 0;
  right_ = std::numeric_limits<uint16_t>::max();
  bottom_ = std::numeric_limits<uint16_t>::max();
}

ExposureSettings::ExposureSettings(double duration_sec,
                                   ShutterAction shutter_action,
                                   READOUT_BINNING_MODE binning_mode,
                                   uint16_t left, uint16_t right, uint16_t top,
                                   uint16_t bottom) {
  exposure_duration_sec_ = duration_sec;
  shutter_action_ = shutter_action;
  binning_mode_ = binning_mode;
  left_ = 0;
  top_ = 0;
  right_ = right;
  bottom_ = bottom;
}
