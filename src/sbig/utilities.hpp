#ifndef UTILITIES_H
#define UTILITIES_H

#define ENUM_TO_STRING(x) #x
#define ID_AND_NAME(x)                                                         \
  case x:                                                                      \
  return ENUM_TO_STRING(x);

float bcd2float(unsigned short bcd);

template<typename T>
int get_bit(T value, int bit_number) {
  int bit = (value & (1 << bit_number)) >> bit_number;
  return bit;
}


#endif // UTILITIES_H
