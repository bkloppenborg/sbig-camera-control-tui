#ifndef SBIG_DETECTOR_H
#define SBIG_DETECTOR_H

// local includes
#include "sbig_st_readout_mode.hpp"
#include "ExposureSettings.hpp"
#include "image.hpp"

// project includes
#include "camera.hpp"

// system includes
#include <sbigudrv.h>
#include <string>
#include <map>
#include <atomic>

class SbigSTDevice;

class SbigSTCamera : public Camera {

protected:
  /// Pointer to the SBIG ST Device.
  SbigSTDevice * mSTDevice = nullptr;

  // 0 = imaging detector, 1 = tracking detector, 2 = external tracking detector
  uint16_t mDetectorId  = 0;

  std::map<READOUT_BINNING_MODE, SBIGReadoutMode> readout_modes_;

  // capabilities
  bool has_electronic_shutter_ = false;
  bool has_physical_shutter_= false;

  std::atomic<bool> do_exposure_;

public:
  ///
  /// \param detector_type, 0 = imaging sensor, 1 = internal tracker, 2 = external tracker
  SbigSTCamera(SbigSTDevice * device, short device_handle, int detector_type,
               GetCCDInfoResults0 device_info0,
               GetCCDInfoResults4 device_info4);
  ~SbigSTCamera();

public:
  std::string ToString();

  image * AcquireImage(ExposureSettings settings);
  void  AbortImage();

  bool ImageInProgress() { return do_exposure_; }

  std::vector<SBIGReadoutMode> GetReadoutModes();

public:
  bool init() { return true; };

  // TODO: connect this
  virtual void setTemperatureTarget(niad::TemperatureType sensor,
                                    bool set_active, double temperature) {};

  // TODO: connect this
  virtual double getTemperature(niad::TemperatureType sensor) { return 0; }
  //
}; // class SbigSTCamera


#endif // SBIG_DETECTOR_H
