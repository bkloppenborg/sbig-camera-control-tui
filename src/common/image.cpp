
#include "image.hpp"
#include "datetime_utilities.hpp"

#include <fitsio2.h>
#include <math.h>


void image::save_to_fits(std::string filename, bool overwrite) {

  fitsfile * fptr;
  int status = 0;

  int bitpix = USHORT_IMG;
  long naxis = 2;
  long naxes[2] = { (long int) width_, (long int) height_ };

  int nelements = width_ * height_;

  // open the file
  fits_create_file(&fptr, filename.c_str(), &status);

  // write the image
  fits_create_img(fptr, bitpix, naxis, naxes, &status);
  fits_write_img(fptr, TUSHORT, 1, nelements, data_.data(), &status);

  std::string t_start = to_iso_8601(exposure_start_);
  fits_write_key(fptr, TSTRING, "DATE-BEG",
                 (void*)t_start.c_str(),
                 "ISO-8601 date-time for start exposure", &status);

  std::string t_end = to_iso_8601(exposure_end_);
  fits_write_key(fptr, TSTRING, "DATE-END",
                 (void *)t_end.c_str(),
                 "ISO-8601 date-time for end exposure",
                 &status);

  std::string t = std::to_string(exposure_duration_sec_);
  fits_write_key(fptr, TSTRING, "EXPTIME",
                 (void*) t.c_str(),
                 "Duration of exposure in seconds",
                 &status);

  fits_write_key(fptr, TSTRING, "FILTER",
                 (void*) filter_name_.c_str(),
                 "Name of photometric filter used",
                 &status);

  fits_write_key(fptr, TSTRING, "DETNAME",
                 (void*) detector_name_.c_str(),
                 "Name of detector used to make the observation",
                 &status);

  fits_write_key(fptr, TSTRING, "OBJECT",
                 (void *) object_name_.c_str(),
                 "Name of detector used to make the observation",
                 &status);

  //
  // Latitude, Longitude, and Altitude
  //
  double t_latitude = latitude_ * 180.0 / M_PI;
  fits_write_key(fptr, TDOUBLE, "LATITUDE",
                 (void *) &t_latitude,
                 "Latitude of observatory (degrees).",
                 &status);
  double t_longitude = longitude_ * 180.0 / M_PI;
  fits_write_key(fptr, TDOUBLE, "LONGITUDE",
                 (void *) &longitude_,
                 "Longitude of observatory (degrees)",
                 &status);
  fits_write_key(fptr, TDOUBLE, "ALTITUDE",
                 (void *) &altitude_,
                 "Altitude of observatory (meters)",
                 &status);

  // close the file
  fits_close_file(fptr, &status);
}
