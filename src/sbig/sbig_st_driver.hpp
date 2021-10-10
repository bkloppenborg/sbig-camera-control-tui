#ifndef SBIG_DRIVER_HPP
#define SBIG_DRIVER_HPP

// local includes
class SbigSTDeviceInfo;
class SbigSTDriver;
class SbigSTDevice;
class ImageData;

// system includes
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <sbigudrv.h>
#include <string>
#include <vector>

/// Information about this driver.
struct SbigSTDriverInfo {
  std::string version; ///< Driver version.
  std::string name; ///< Driver Name
  int maxRequests; ///< Maximum number of requests that can be sent to this driver.
}; // struct SbigSTDriverInfo

/// Singleton class to interface with the SBIG Universal driver.
class SbigSTDriver
{
public:
  /// Get access to the singleton.
  static SbigSTDriver& GetInstance()
  {
    static SbigSTDriver instance;
    return instance;
  }

private:
  /// Default constructor
  SbigSTDriver();
  /// Copy constructor.
  SbigSTDriver(SbigSTDriver const &);
  /// Default destructor.
  ~SbigSTDriver();

  /// Equal operator override.
  void operator=(SbigSTDriver const &);

private:
  std::mutex driver_access_mutex_; ///< Mutex for driver access
  std::mutex device_readout_mutex_; ///< Mutex for detector readout operatoins.
  /// Map containing handles and active devices.
  std::map<short, std::shared_ptr<SbigSTDevice>> active_devices_;
  short active_device_handle_; ///< Handle for the current active device

  std::atomic<bool> do_readout_; ///< Boolean to indicate if readouts should occur.

private:
  /// Open the driver
  void Open();
  /// Close the driver
  void Close();
  /// Run a command against the driver.
  void RunCommand(short sbig_command, void *params, void *results);

public:

  /// Get a vector containing strings of supported camera models.
  /// \return Vector of support camera model names.
  static std::vector<std::string> GetSupportedCameras();

  /// Get a vector containing strings of supported filter wheel. models.
  /// \return Vector of support filter wheel. model names.
  static std::vector<std::string> GetSupportedFilterWheels();

  /// Get information about this driver.
  /// \return SbigSTDriverInfo struct with information about this driver.
  SbigSTDriverInfo GetInfo();

  /// Get a list of devices presently manged by this driver.
  /// \return A vector of active devices.
  std::vector<SbigSTDeviceInfo> GetDeviceList();

  /// Find a SBIG camera and filter wheel by name
  /// \param device_name Camera model.
  /// \param filter_wheel_name Model of the filter wheel.
  /// \return SBIG device information matching the requested items. May be "blank"
  SbigSTDeviceInfo FindDevice(std::string device_name, std::string filter_wheel_name = "");

  /// Opens a SBIG device matching the provided information
  /// \param info Specifications on the device to be opened.
  /// \return A shared pointer ot the device.
  std::shared_ptr<SbigSTDevice> OpenDevice(const SbigSTDeviceInfo & info);

  /// Closes the connection to the specified device. Frees all related resources.
  /// Note: After this point any shared pointers to the device will be invalid.
  /// \param device The device to be closed.
  void CloseDevice(std::shared_ptr<SbigSTDevice> device);

  /// Runs a SBIG command. See SBIG universal driver documentation for further
  /// information.
  void RunCommand(short sbig_command, void *params, void *results, short handle);

  /// Produces a string describing this object.
  std::string ToString();

  /// Instruct the driver to read data out from the specified camera
  /// \param device_handle Handle to the device
  /// \param detector_id the detector from which data will be read
  /// \param bin_mode binning mode
  /// \param top The coordinate for the top-most pixel for the readout.
  /// \param left The coordinate for the left-most pixel for the readout.
  /// \param width The width of the resulting image.
  /// \param height The height of the resulting image.
  ImageData * DoReadout(short device_handle, short detector_id,
                    uint16_t bin_mode,
                    uint16_t top, uint16_t left,
                    uint16_t width, uint16_t height);

  /// Abort all active readout operations.
  void AbortReadout();

  //
}; // class SSbigSTDriver

#endif // SBIG_DRIVER_HPP
