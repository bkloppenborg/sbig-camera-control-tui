#include "sbig_st_camera.hpp"

SbigSTCamera::SbigSTCamera() {
}

SbigSTCamera::~SbigSTCamera() {
}

bool SbigSTCamera::init() {
  return true;
}

void SbigSTCamera::setTemperatureTarget(niad::TemperatureType sensor,
                                        bool set_active,
                                        double temperature) {
  
}

double SbigSTCamera::getTemperature(niad::TemperatureType sensor) {
  return 0;
}
