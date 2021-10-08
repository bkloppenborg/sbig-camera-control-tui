
float bcd2float(unsigned short bcd) {
  unsigned char b1 = bcd >> 8;
  unsigned char b2 = bcd;

  float f1 = 10 * (b1 >> 4);
  float f2 = b1 & 0x0F;
  float f3 = 0.10 * (b2 >> 4);
  float f4 = 0.01 * (b2 & 0x0F);

  return (f1 + f2 + f3 + f4);
}
