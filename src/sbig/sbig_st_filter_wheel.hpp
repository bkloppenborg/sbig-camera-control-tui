#ifndef SBIG_FILTER_WHEEL_H
#define SBIG_FILTER_WHEEL_H

// local includes
class SbigSTDevice;

// project includes
#include "filter_wheel.hpp"

// system includes
#include <map>
#include <sbigudrv.h>
#include <string>
#include <vector>

/// Class representing a SBIG filter wheel.
class SbigSTFilterWheel : public FilterWheel {
protected:

  /// Pointer to the SBIG device to which this filter wheel is connected.
  SbigSTDevice * device_ = nullptr;

  /// Model of the filter wheel.
  CFW_MODEL_SELECT model_ = CFWSEL_UNKNOWN;

public:
  /// Default constructor
  /// \param device SBIG device to which this filter wheel is connected
  /// \param filter_wheel_id Name of this filter wheel (typically a model number)
  /// \param slot_to_filter_name A map which associates a slot number to filter name.
  SbigSTFilterWheel(SbigSTDevice * device,
                  std::string filter_wheel_id,
                  std::map<size_t, std::string> slot_to_filter_name);
  /// Default destructor.
  ~SbigSTFilterWheel();

private:
  /// Perform all setup operations for the filter wheel.
  void Setup(std::string filter_wheel_id, std::map<size_t, std::string> slot_to_filter_name);
  /// Open a connection to the filter wheel.
  void Open();
  /// Close the connection to the filter wheel.
  void Close();

public:
  /// Get a list of filter wheels supported by this class.
  static std::vector<std::string> GetSupportedFilterWheels();

public:
  /// See filter_wheel.hpp
  void Init();

public:
  /// See filter_wheel.hpp
  virtual bool init() { return true; }
  /// See filter_wheel.hpp
  virtual double setFilter(size_t filter_slot_id);
  /// See filter_wheel.hpp
  virtual double setFilter(std::string filter_name);

  //
}; // SbigSTFilterWheel

#endif // SBIG_FILTER_WHEEL_H
