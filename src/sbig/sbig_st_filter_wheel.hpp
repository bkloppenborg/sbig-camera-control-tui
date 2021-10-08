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


struct SbigSTFilterWheelInfo {
  std::string model_name = "";
  uint16_t max_slots     = 0;
  uint16_t active_slot   = 0;
  std::map<size_t, std::string> slot_to_filter_name;
}; // struct SbigSTFilterWheelInfo

class SbigSTFilterWheel : public FilterWheel {
protected:
  SbigSTDevice * device_ = nullptr;

  SbigSTFilterWheelInfo info_;

  CFW_MODEL_SELECT model_ = CFWSEL_UNKNOWN;

public:
  SbigSTFilterWheel(SbigSTDevice * device,
                  std::string filter_wheel_id,
                  std::map<size_t, std::string> slot_to_filter_name);
  ~SbigSTFilterWheel();

private:
  void Setup(std::string filter_wheel_id, std::map<size_t, std::string> slot_to_filter_name);
  void Open();
  void Close();

public:
  static std::vector<std::string> GetSupportedFilterWheels();

public:
  void Init();
  virtual double setFilter(size_t filter_slot_id);
  virtual double setFilter(std::string filter_name);

  std::map<size_t, std::string> GetSlotToFilterMap() { return info_.slot_to_filter_name; }
  std::string GetActiveFilterName();
  size_t GetActiveFilterSlot();

public:
  /// overrides
  virtual bool init() { return true; }

  //
}; // SbigSTFilterWheel

#endif // SBIG_FILTER_WHEEL_H
