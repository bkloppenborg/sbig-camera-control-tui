#ifndef SBIG_READOUT_MODE_H
#define SBIG_READOUT_MODE_H

#include <sbigudrv.h>
#include <cstdint>

#include <string>
#include <map>

struct SBIGReadoutMode {
  READOUT_BINNING_MODE binning_mode; /// Binning mode identifier.
  uint16_t max_width  = 0; /// number of pixels in x direction
  uint16_t max_height = 0; /// number of pixels in y direction
  double gain         = 1.0; /// gain with this readout mode
  double pixel_width  = 0; /// effective x-size of pixels in micron
  double pixel_height = 0; /// effective y-size of pixels in microns
  std::string name = "INVALID_MODE"; /// Name for the mode.
};

std::map<int, std::string> SBIGReadoutMap();
std::string SBIGReadoutModeToName(int id);
int SBIGReadoutNameToMode(const std::string & name);

#endif // SBIG_READOUT_MODE_H
