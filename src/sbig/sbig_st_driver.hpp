#ifndef SBIG_DRIVER_HPP
#define SBIG_DRIVER_HPP

#include <mutex>
#include <sbigudrv.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>

class SbigSTDeviceInfo;
class SbigSTDriver;
class SbigSTDevice;
class ImageData;
class ExposureSettings;

struct SbigSTDriverInfo {
  std::string version;
  std::string name;
  int maxRequests;
}; // class SbigSTDriverInfo

class SbigSTDriver
{
public:
  static SbigSTDriver& GetInstance()
  {
    static SbigSTDriver instance;
    return instance;
  }

private:
  SbigSTDriver();
  SbigSTDriver(SbigSTDriver const &);
  void operator=(SbigSTDriver const &);
  ~SbigSTDriver();

private:
  std::mutex driver_access_mutex_;
  std::mutex device_readout_mutex_;
  std::map<short, std::shared_ptr<SbigSTDevice>> active_devices_;
  short active_device_handle_; // device handle

  std::atomic<bool> do_readout_;

private:
  void Open();
  void Close();
  void RunCommand(short sbig_command, void *params, void *results);

public:

  static std::vector<std::string> GetSupportedCameras();
  static std::vector<std::string> GetSupportedFilterWheels();
  SbigSTDriverInfo GetInfo();

  std::vector<SbigSTDeviceInfo> GetDeviceList();
  SbigSTDeviceInfo FindDevice(std::string device_name, std::string filter_wheel_name = "");

  std::shared_ptr<SbigSTDevice> OpenDevice(const SbigSTDeviceInfo & info);
  void CloseDevice(std::shared_ptr<SbigSTDevice> device);

  void RunCommand(short sbig_command, void *params, void *results, short handle);
  std::string ToString();

  ImageData * DoReadout(short device_handle, short detector_id,
                    uint16_t bin_mode,
                    uint16_t top, uint16_t left,
                    uint16_t width, uint16_t height);
  void AbortReadout();
  //
}; // class SSbigSTDriver

#endif // SBIG_DRIVER_HPP
