#include "sbig_st_readout_mode.hpp"
#include "utilities.hpp"

#include <cstddef>
#include <string>

std::map<int, std::string> SBIGReadoutMap() {
  using namespace std;

  map<int, string> output;

  output[RM_1X1]          = string(ENUM_TO_STRING(RM_1X1));
  output[RM_2X2]          = string(ENUM_TO_STRING(RM_2X2));
  output[RM_3X3]          = string(ENUM_TO_STRING(RM_3X3));
  output[RM_NX1]          = string(ENUM_TO_STRING(RM_NX1));
  output[RM_NX2]          = string(ENUM_TO_STRING(RM_NX2));
  output[RM_NX3]          = string(ENUM_TO_STRING(RM_NX3));
  output[RM_1X1_VOFFCHIP] = string(ENUM_TO_STRING(RM_1X1_VOFFCHIP));
  output[RM_2X2_VOFFCHIP] = string(ENUM_TO_STRING(RM_2X2_VOFFCHIP));
  output[RM_3X3_VOFFCHIP] = string(ENUM_TO_STRING(RM_3X3_VOFFCHIP));
  output[RM_9X9]          = string(ENUM_TO_STRING(RM_9X9));
  output[RM_NXN]          = string(ENUM_TO_STRING(RM_NXN));

  return output;
}


std::string SBIGReadoutModeToName(int id) {
  auto readout_map = SBIGReadoutMap();

  return readout_map.at(id);

}

int SBIGReadoutNameToMode(const std::string & name) {

  auto readout_map = SBIGReadoutMap();
  for(auto it: readout_map) {
    auto i = it.first;
    auto n = it.second;
    if(n == name)
      return i;
  }

  return -1;
}
