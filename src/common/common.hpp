#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>

#define ENUM_TO_STRING(x) #x
#define ID_AND_NAME(x)                                                         \
  case x:                                                                      \
  return ENUM_TO_STRING(x);

/// Converts a decimal value to hexadecimal with a minimum field width.
/// \param value Decimal value to be encoded
/// \param min_width The minimum output width.
/// \return A vector containing the encoded value.
std::vector<unsigned char> dec2hex(long value, size_t min_width = 8);

/// Converts a hexadecimal value to a decimal value
/// \param data The hexadecimal value to decode
/// \return A The decimal value.
long hex2dec(const std::vector<unsigned char> &data);

/// Converts a binary coded decimal to a floating point value
/// \param bcd binary coded decimal value.
float bcd2float(unsigned short bcd);

/// Gets the Nth bit from the specified value.
/// \param value Input value
/// \param bit_number The bit to extract.
template<typename T>
int get_bit(T value, int bit_number) {
  int bit = (value & (1 << bit_number)) >> bit_number;
  return bit;
}

#endif // COMMON_H
