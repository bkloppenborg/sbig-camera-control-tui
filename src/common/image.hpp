#ifndef IMAGE_H
#define IMAGE_H

#include <chrono>
#include <vector>
#include <string>

class image {

public:
  std::vector<uint16_t> data_;
  size_t width_   = 0;
  size_t height_  = 0;
  bool   aborted_ = false;

  // exposure information
  std::string filter_name_ = "";   ///< Name of photometric filter
  std::string detector_name_ = ""; ///< Name of detector/camera
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_start_;
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_end_;
  double exposure_duration_sec_ = 0.0;

  // object information
  std::string object_name_ = ""; ///< Name of object under observation.

  // observatory information
  double latitude_  = 0; ///< Latitude (radians).
  double longitude_ = 0; ///< Longitude (radians).
  double altitude_  = 0; ///< Altitude above mean sea level (meters).

public:
  image(size_t width, size_t height) {
    width_  = width;
    height_ = height;
    data_.resize(width * height);
  }

  ~image() {
  }

  void save_to_fits(std::string filename, bool overwrite = false);

  //
}; 


#endif // RAW_IMAGE_H
