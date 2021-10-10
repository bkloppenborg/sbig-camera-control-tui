#ifndef SBIG_READOUT_MODE_H
#define SBIG_READOUT_MODE_H

#include <sbigudrv.h>
#include <cstdint>

#include <string>
#include <map>

/// Structure for storing SBIG readout configuration.
struct SBIGReadoutMode {
  READOUT_BINNING_MODE binning_mode; ///< Binning mode identifier.
  uint16_t max_width  = 0; ///< number of pixels in x direction
  uint16_t max_height = 0; ///< number of pixels in y direction
  double gain         = 1.0; ///< gain with this readout mode
  double pixel_width  = 0; ///< effective x-size of pixels in micron
  double pixel_height = 0; ///< effective y-size of pixels in microns
  std::string name = "INVALID_MODE"; ///< Name for the mode.
};

/// Get the readout map
std::map<int, std::string> SBIGReadoutMap();

/// Convert a readout mode to a name.
std::string SBIGReadoutModeToName(int id);

/// Convert a name to a specific readout mode
int SBIGReadoutNameToMode(const std::string & name);

#endif // SBIG_READOUT_MODE_H
