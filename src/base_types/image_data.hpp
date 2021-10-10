#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <chrono>
#include <vector>
#include <string>

class ImageData {

public:
  std::vector<uint16_t> data;
  size_t width   = 0;
  size_t height  = 0;
  bool   aborted = false;

  // exposure information
  std::string filter_name = "";   ///< Name of photometric filter
  std::string detector_name = ""; ///< Name of detector/camera
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_start;
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_end;
  double exposure_duration_sec = 0.0;

  // object information
  std::string object_name = ""; ///< Name of object under observation.

  // observatory information
  double latitude  = 0; ///< Latitude (radians).
  double longitude = 0; ///< Longitude (radians).
  double altitude  = 0; ///< Altitude above mean sea level (meters).

  // observing conditions
  double temperature = 100; ///< Sensor temperature in Celsius

  // pointing information
  bool ra_dec_set = false;
  double ra = 0;
  double dec = 0;

  bool azm_alt_set = false;
  double azm = 0;
  double alt = 0;


public:
  ImageData(size_t width, size_t height) {
    this->width  = width;
    this->height = height;
    this->data.resize(width * height);
  }

  ~ImageData() {
  }

  void save_to_fits(std::string filename, bool overwrite = false);

  //
};


#endif // RAW_IMAGEDATA_H
