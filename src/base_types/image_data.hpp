#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <chrono>
#include <vector>
#include <string>

/// A class for storing and managing image data.
class ImageData {

public:
  std::vector<uint16_t> data; ///< Vector containing the image data.
  size_t width   = 1; ///< Width of the image in units of pixel.
  size_t height  = 1; ///< Height of the image in units of pixels.
  size_t depth   = 1; ///< Depth of the image in units of layers.
  bool   aborted = false; ///< Whether or not the readout for this image was aborted.

  // exposure information
  std::string filter_name = "";   ///< Name of photometric filter
  std::string detector_name = ""; ///< Name of detector/camera

  /// Time at which the exposure began.
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_start;

  /// Time at which the exposure ended.
  std::chrono::time_point<std::chrono::high_resolution_clock> exposure_end;

  /// Time at which the readout began.
  std::chrono::time_point<std::chrono::high_resolution_clock> readout_start;

  /// Time at which the readout ended.
  std::chrono::time_point<std::chrono::high_resolution_clock> readout_end;

  /// Duration of the exposure in units of seconds.
  double exposure_duration_sec = 0.0;

  // object information
  std::string catalog_name = ""; ///< Name of catalog from where the obsevation comes.
  std::string object_name = ""; ///< Name of object under observation.

  // observatory information
  double latitude  = 0; ///< Latitude of the observatory (radians).
  double longitude = 0; ///< Longitude of the observatory (radians).
  double altitude  = 0; ///< Altitude above mean sea level of the observatory (meters).

  // observing conditions
  double temperature = 100; ///< Sensor temperature in Celsius

  // pointing information
  bool ra_dec_set = false; ///< Whether or not the RA/DEC coordinates are set.
  double ra       = 0; ///< RA coordinate of the image center (radians).
  double dec      = 0; ///< DEC coordinate of the image center (radians).

  bool azm_alt_set = false; ///< Whether or not the AZM/ALT coordinates are set.
  double azm       = 0; ///< AZM coordinate of the image center (radians).
  double alt       = 0; ///< ALT coordinate of the image center (radians).


public:
  /// Default constructor.
  ImageData(size_t width, size_t height, size_t depth=1) {
    this->width  = width;
    this->height = height;
    this->depth  = depth;
    this->data.resize(width * height * depth);
  }

  /// Default destruct.
  ~ImageData() {
  }

  /// Saves the file to a FITS image.
  /// \param filename Name of the output file.
  /// \param overwrite Whether or not the file should overwrite an existing image.
  void saveToFITS(std::string filename, bool overwrite = false);

  //
};


#endif // RAW_IMAGEDATA_H
