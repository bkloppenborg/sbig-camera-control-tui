#include "camera.hpp"

Camera::Camera() {

}

Camera::~Camera() {

}

std::vector<niad::CameraCapability> Camera::getCapabilities() {
  return mCapabilities;
}

std::vector<int64_t> Camera::getPixelCount() {
  return mPixelCount;
}

std::vector<double> Camera::getPixelSize() {
  return mPixelSize;
}

std::vector<double> Camera::getExposureMinMax() {
  std::vector<double> min_max;
  min_max.push_back(mExposureDurationMin);
  min_max.push_back(mExposureDurationMax);
  return min_max;
}

std::vector<double>Camera::getGainMinMax() {
  std::vector<double> min_max;
  min_max.push_back(mGainMin);
  min_max.push_back(mGainMax);
  return min_max;
}

niad::CameraSensorType Camera::getSensorType() {
  return mSensorType;
}

niad::BitDepth Camera::getBitDepth() {
  return mBitDepth;
}

niad::CameraSensorColorType Camera::getSensorColorType() {
  return mColorType;
}

std::vector<niad::CameraReadoutMode> Camera::getReadoutModes() {
  return mReadoutModes;
}
