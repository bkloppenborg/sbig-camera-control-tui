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

  /// Current active filter slot. To be set and maintained by subclasses.
  uint16_t mActiveSlot = 0;

  /// Total number of slots for filters. To be set and maintained by subclasses.
  uint16_t mMaxActiveSlots = 0;

  /// A map associating a filter ID to a filter name. TO be populated by subclasses.
  std::map<size_t, std::string> mSlotToFilterName;

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
  /// \param filter_slot_number The integer index of the filter slot to change to (zero indexed)
  /// \return Delay, in seconds, to change the filter wheel.
  virtual double setFilter(size_t filter_slot_number) = 0;

  /// Gets a vector containing the names of the presently installed filters.
  /// \return Vector containing filter names.
  std::vector<std::string> getFilterNames();

  /// Returns a map that associates a filter ID to a filter name.
  std::map<size_t, std::string> getSlotToFilterMap();

  /// Gets the name of the presently active filter
  std::string getActiveFilterName();

  /// Gets the index of the presently active filter
  size_t getActiveFilterSlot();

  //
}; // FilterWheel


#endif // FILTER_WHEEL_H
