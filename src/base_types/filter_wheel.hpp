#ifndef FILTER_WHEEL_H
#define FILTER_WHEEL_H

// local includes
#include "device.hpp"

// external includes
#include "filter-wheel-enums.pb.h"

/// class
class FilterWheel : public Device {

public:
  // constructor
  FilterWheel();
  // destructor
  virtual ~FilterWheel();

protected:
  /// Vector containing the FilterWheel's capabilities. Should be cleared and populated
  /// by a subclass.
  std::vector<niad::FilterWheelCapability> mCapabilities = {niad::FILTER_WHEEL_CAPABILITY_NONE};

public:
  ///
  /// Pure virtual functions to be implemented by sublclasses
  ///

  /// See device.hpp
  virtual bool init() = 0;

public:
  /// Get the FilterWheel's capabilities.
  virtual std::vector<niad::FilterWheelCapability> getCapabilities();

  /// Change the filter wheel position.
  /// \param filter_name The name of the filter to select.
  /// \return Delay, in seconds, to change the filter wheel.
  virtual double setFilter(std::string filter_name) = 0;

  /// Change the filter wheel position.
  /// \param filter_name The integer index of the filter slot to change to (zero indexed)
  /// \return Delay, in seconds, to change the filter wheel.
  virtual double setFilter(size_t filter_slot_number) = 0;

  //
}; // FilterWheel


#endif // FILTER_WHEEL_H
