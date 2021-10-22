#ifndef SBIG_DETECTOR_H
#define SBIG_DETECTOR_H

// local includes
#include "sbig_st_readout_mode.hpp"

// project includes
#include "camera.hpp"

// system includes
#include <sbigudrv.h>
#include <string>
#include <map>
#include <atomic>

class SbigSTDevice;

/// Class representing a SBIG camera.
class SbigSTCamera : public Camera {

protected:
  /// Pointer to the SBIG ST Device.
  SbigSTDevice * mSTDevice = nullptr;

  /// Identifier for the detector. From SBIG documentation:
  /// 0 = imaging detector, 1 = tracking detector, 2 = external tracking detector
  uint16_t mDetectorId  = 0;

  /// Map containing nominal readout settings.
  std::map<niad::CameraReadoutMode, SBIGReadoutMode> mReadoutSettings;

  bool has_electronic_shutter_ = false; ///< True if there is an electronic shutter
  bool has_physical_shutter_   = false; ///< True if there is a physical shutter.

  std::atomic<bool> do_exposure_; ///< Flag to indicate if an exposure should be continued.

public:
  /// Default constructor.
  /// \param device Pointer to the underlying SBIG device.
  /// \param device_handle Handle to the underlying SBIG device.
  /// \param detector_type 0 = imaging sensor, 1 = internal tracker, 2 = external tracker
  /// \param device_info0 SBIG-specific information.
  /// \param device_info4 SBIG-specific information.
  SbigSTCamera(SbigSTDevice * device, short device_handle, int detector_type,
               GetCCDInfoResults0 device_info0,
               GetCCDInfoResults4 device_info4);
  /// Default destructor.
  ~SbigSTCamera();

public:
  /// Get a string describing this object.
  std::string ToString();

  /// Stop an image in progress.
  void  abortExposure();

  /// Flag to indicate if an exposure is in progress.
  bool ImageInProgress() { return do_exposure_; }

  /// Returns a vector of supported readout modes.
  std::vector<SBIGReadoutMode> GetReadoutModes();

public:
  /// See camera.hpp
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
