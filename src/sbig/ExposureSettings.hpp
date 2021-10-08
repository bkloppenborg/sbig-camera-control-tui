#ifndef EXPOSURE_SETTINGS_H
#define EXPOSURE_SETTINGS_H

#include "sbig_st_readout_mode.hpp"

#include <sbigudrv.h>
#include <cstdint>

class ExposureSettings {

public:
  enum ShutterAction {
    SHUTTER_LEAVE_ALONE,
    SHUTTER_OPEN_CLOSE,
    SHUTTER_CLOSE_CLOSE,
  };

public:
  READOUT_BINNING_MODE binning_mode_ = RM_1X1;
  ShutterAction shutter_action_ = SHUTTER_LEAVE_ALONE;
  double exposure_duration_sec_ = 0.0;
  uint16_t left_ = 0;
  uint16_t right_ = 1;
  uint16_t top_ = 0;
  uint16_t bottom_ = 1;

public:
  ExposureSettings(double duration_sec,
                   ShutterAction shutter_action = SHUTTER_OPEN_CLOSE,
                   READOUT_BINNING_MODE binning_mode = READOUT_BINNING_MODE::RM_1X1);
  ExposureSettings(double duration_sec,
                   ShutterAction shutter_action,
                   READOUT_BINNING_MODE binning_mode,
                   uint16_t left, uint16_t right,
                   uint16_t top,  uint16_t bottom);
};

#endif // EXPOSURE_SETTINGS_H
