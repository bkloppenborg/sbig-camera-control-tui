#ifndef SBIG_ST_CAMERA_H
#define SBIG_ST_CAMERA_H

// local includes

// project includes
#include "camera.hpp"

/// Class encapsulating a SBIG ST Camera
class SbigSTCamera : public Camera {

public:
  /// constructor
  SbigSTCamera();

  // destructor
  ~SbigSTCamera();

public:
  //
  // Overrides for camera.hpp
  //
  /// See camera.hpp
  virtual bool init();

  /// See camera.hpp
  virtual void setTemperatureTarget(niad::TemperatureType sensor,
                                    bool set_active,
                                    double temperature);

  /// See camera.hpp
  virtual double getTemperature(niad::TemperatureType sensor);

}; // SbigSTCamera


#endif // SBIG_ST_CAMERA_H
