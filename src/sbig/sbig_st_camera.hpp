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

  std::map<niad::CameraReadoutMode, SBIGReadoutMode> mReadoutSettings;

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

  void  AbortImage();

  bool ImageInProgress() { return do_exposure_; }

  std::vector<SBIGReadoutMode> GetReadoutModes();

public:
  bool init() { return true; };

  /// See camera.hpp
  virtual void setTemperatureTarget(niad::TemperatureType target,
                                    bool set_active, double temperature);

  /// See camera.hpp
  virtual double getTemperatureTarget(niad::TemperatureType target);

  /// See camera.hpp
  virtual double getTemperature(niad::TemperatureType temperature_type);

  /// See camera.hpp
  virtual ImageData *acquireImage(double duration,
          niad::CameraReadoutMode readout_mode = niad::CAMERA_READOUT_MODE_1X1,
          niad::CameraShutterAction shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE);

  /// See camera.hpp.
  virtual ImageData *acquireImage(double duration,
          uint16_t left, uint16_t right,
          uint16_t top,  uint16_t bottom,
          niad::CameraReadoutMode readout_mode = niad::CAMERA_READOUT_MODE_1X1,
          niad::CameraShutterAction shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE);

  //
}; // class SbigSTCamera


#endif // SBIG_DETECTOR_H
