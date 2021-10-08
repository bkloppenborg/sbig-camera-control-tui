#ifndef SBIG_ERRORS_HPP
#define SBIG_ERRORS_HPP

#include <stdexcept>
#include <cstring>
#include <iostream>

const char *SBIGErrorToName(int id);

#define SBIG_CHECK_STATUS(status)                                       \
  if (status != CE_NO_ERROR) {                                          \
    std::cout << "Location : " << __FILE__ << ":" << __LINE__ << std::endl; \
    throw std::runtime_error(SBIGErrorToName(status));                  \
  }

#endif // SBIG_ERRORS_HPP
