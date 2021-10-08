#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>

#include "common.hpp"

std::vector<unsigned char> dec2hex(long value, size_t min_width) {
  using namespace std;
  vector<unsigned char> output;

  stringstream ss;
  ss << std::hex << std::uppercase << setfill('0') << setw(min_width) << value;
  string tmp(ss.str());
  copy(tmp.begin(), tmp.end(), std::back_inserter(output));

  return output;
}

long hex2dec(const std::vector<unsigned char> &data) {
  using namespace std;
  long output;

  stringstream ss;
  ss << string(data.begin(), data.end());
  ss >> std::hex >> output;

  return output;
}

float bcd2float(unsigned short bcd) {
  unsigned char b1 = bcd >> 8;
  unsigned char b2 = bcd;

  float f1 = 10 * (b1 >> 4);
  float f2 = b1 & 0x0F;
  float f3 = 0.10 * (b2 >> 4);
  float f4 = 0.01 * (b2 & 0x0F);

  return (f1 + f2 + f3 + f4);
}

