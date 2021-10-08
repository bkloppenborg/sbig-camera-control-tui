#ifndef VERSION_NUMBER_H
#define VERSION_NUMBER_H

// Some GNU systems have aliases
//  gnu_dev_major() -> major
//  gnu_dev_minor() -> minor
// which are not specified in POSIX, but are nevertheless present. Here we push
// them out of the way to permit the VersionNumber class to have members named
// major, minor.

#pragma push_macro("major")
#undef major
#pragma push_macro("minor")
#undef minor

#include <string>
#include <tuple>

/// Class for storing and querying version numbers.
class VersionNumber {

public:

  /// default constructor
  VersionNumber(): major(-1), minor(-1), patch(-1){};
  /// construct a version number from major, minor, and patch data
  VersionNumber(int major, int minor = -1, int patch = -1)
      : major(major), minor(minor), patch(patch){};

  int major = -1; ///< version major number
  int minor = -1; ///< version minor number
  int patch = -1; ///< version patch number

  /// Convert this object to a string representation.
  std::string toString() {
    std::string output = std::to_string(major);
    if(minor > -1)
      output += "." + std::to_string(minor);
    if(patch > -1)
      output += "." + std::to_string(patch);

    return output;
  }

  /// Operator <
  friend bool operator<(const VersionNumber &l, const VersionNumber &r) {
    return std::tie(l.major, l.minor, l.patch) <
           std::tie(r.major, r.minor, r.patch);
  }

  /// Operator >
  friend bool operator>  (const VersionNumber &lhs, const VersionNumber &rhs) { return rhs < lhs; }
  /// Operator <=
  friend bool operator<= (const VersionNumber &lhs, const VersionNumber &rhs) { return !(lhs > rhs); }
  /// Operator >=
  friend bool operator>= (const VersionNumber &lhs, const VersionNumber &rhs) { return !(lhs < rhs); }
  /// Operator ==
  friend bool operator==(const VersionNumber &l, const VersionNumber &r) {
    return std::tie(l.major, l.minor, l.patch) ==
           std::tie(r.major, r.minor, r.patch);
  }

  //
}; // class VersionNumber

#pragma pop_macro("minor")
#pragma pop_macro("major")

#endif // VERSION_NUMBER_H
