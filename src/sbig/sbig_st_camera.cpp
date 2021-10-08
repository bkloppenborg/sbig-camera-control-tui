#include "sbig_st_camera.hpp"
#include "utilities.hpp"
#include "sbig_st_driver.hpp"
#include "sbig_st_device.hpp"

#include <sstream>
#include <iostream>
#include <thread>
#include <cmath>

SbigSTCamera::SbigSTCamera(SbigSTDevice *device, short device_handle,
                           int detector_id, GetCCDInfoResults0 device_info0,
                           GetCCDInfoResults4 device_info4)
  : mSTDevice(device),
    mDetectorId(detector_id),
    do_exposure_(false)
{
  // Populate Device::mName
  std::stringstream ss;
  ss << device_info0.name << " " << detector_id;
  mName = ss.str();

  // Populate mDevice::mVersion
  double firmware_version = bcd2float(device_info0.firmwareVersion);
  double v_major = 0;
  double v_minor = 0;
  double v_patch = 0;
  v_minor = modf(firmware_version, &v_major);
  mVersion = VersionNumber(int(v_major), int(v_minor * 100), int(v_patch));

  // Determine the number and size of pixels.
  int64_t pixel_count_x = 0;
  int64_t pixel_count_y = 0;
  double pixel_size_x = 1000; // microns
  double pixel_size_y = 1000; // microns
  for (int i = 0; i < device_info0.readoutModes; i++) {
    auto info = device_info0.readoutInfo[i];

    // create a readout mode object.
    SBIGReadoutMode tmp;
    tmp.binning_mode = (READOUT_BINNING_MODE)info.mode;
    tmp.gain = bcd2float(info.gain);
    tmp.max_height = info.height;
    tmp.max_width = info.width;
    tmp.pixel_height = bcd2float(info.pixelHeight);
    tmp.pixel_width = bcd2float(info.pixelWidth);
    tmp.name = SBIGReadoutModeToName(info.mode);
    readout_modes_[tmp.binning_mode] = tmp;

    // Find the number of pixels
    if (tmp.max_width > pixel_count_x)
      pixel_count_x = tmp.max_width;
    if (tmp.max_height > pixel_count_y)
      pixel_count_y = tmp.max_height;

    // Find the pixel size (in microns).
    if (tmp.pixel_width < pixel_size_x)
      pixel_size_x = tmp.pixel_width;
    if (tmp.pixel_height < pixel_size_y)
      pixel_size_y = tmp.pixel_height;
  }

  // Populate the pixel count.
  mPixelCount.clear();
  mPixelCount.push_back(pixel_count_x);
  mPixelCount.push_back(pixel_count_y);

  // Populate the pixel size.
  mPixelSize.clear();
  mPixelSize.push_back(pixel_size_x);
  mPixelSize.push_back(pixel_size_y);

  // Set default exposure durations. These are overridden in a block below.
  if (detector_id == 0) {
    mExposureDurationMin = 0.12;
    mExposureDurationMax = 167777.16;
  } else {
    mExposureDurationMin = 0.12;
    mExposureDurationMax = 655.35;
  }

  // Set default camera capabilities.
  mCapabilities.clear();
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_CHANGE_EXPOSURE_DURATION);
  // Technically all SBIG sensors support changing the gain, but they are really
  // optimized to be used at the factory-default gain. Disable this option.
  //mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_CHANGE_GAIN);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_CHANGE_READOUT_MODE);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_READOUT_SUBARRAY);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_SET_EXPOSURE_SHUTTER_ACTION);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_SET_SENSOR_TEMPERATURE);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_GET_SENSOR_TEMPERATURE);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_STORE_IMAGES_LOCALLY);
  mCapabilities.push_back(niad::CAMERA_CAPABILITY_CAN_STORE_IMAGES_NETWORK);

  // Set readout modes.
  mReadoutModes.clear();
  mReadoutModes.push_back(niad::CAMERA_READOUT_MODE_1x1);
  mReadoutModes.push_back(niad::CAMERA_READOUT_MODE_2x2);
  mReadoutModes.push_back(niad::CAMERA_READOUT_MODE_3x3);
  mReadoutModes.push_back(niad::CAMERA_READOUT_MODE_9X9);

  // Set default sensor configuration.
  mSensorType = niad::CameraSensorType::CAMERA_SENSOR_TYPE_CCD;
  mColorType = niad::CameraSensorColorType::CAMERA_SENSOR_COLOR_MONOCHROMATIC;

  // Set shutter actions.
  // Only permit the main camera to move the shutter.
  mShutterActions.clear();
  if(detector_id == 0) {
    mShutterActions.push_back(niad::CAMERA_SHUTTER_ACTION_OPEN_OPEN);
    mShutterActions.push_back(niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE);
    mShutterActions.push_back(niad::CAMERA_SHUTTER_ACTION_OPEN_OPEN);
  } else {
    // technically either camera can move the shutter, but lets disable that ability.
    mShutterActions.push_back(niad::CAMERA_SHUTTER_ACTION_NONE);
  }

  // Image actions
  mImageActions.clear();
  mImageActions.push_back(niad::CameraImageAction::CAMERA_IMAGE_ACTION_SEND);
  mImageActions.push_back(niad::CameraImageAction::CAMERA_IMAGE_ACTION_STORE);
  mImageActions.push_back(niad::CameraImageAction::CAMERA_IMAGE_ACTION_SEND_AND_STORE);

  // Bit depth
  mBitDepth = niad::BIT_DEPTH_16_BIT;

  // Override or supplement anything above that isn't correct on a
  // camera-by-camera basis.
  switch (device_info0.cameraType) {
  case ST7_CAMERA:
  case ST8_CAMERA:
  case ST5C_CAMERA:
  case ST9_CAMERA:
  case STV_CAMERA:
  case ST10_CAMERA:
  case ST1K_CAMERA:
    // These cameras have physical shutters.
    if(detector_id == 0)
      mCapabilities.push_back(niad::CAMERA_CAPABILITY_HAS_PHYSICAL_SHUTTER);

    break;
  case ST402_CAMERA:
    // These cameras have a shorter minimum exposure of 40 ms. See SBIGUDRV.doc
    // page 12.
    mExposureDurationMin = 0.040;

    // These cameras have physical shutters.
    if(detector_id == 0)
      mCapabilities.push_back(niad::CAMERA_CAPABILITY_HAS_PHYSICAL_SHUTTER);

    break;
  case ST2K_CAMERA:
  case STL_CAMERA:
    // Minimum exposure duration of 0.001 seconds, see SBIGUDRV.doc pg 12
    mExposureDurationMin = 0.001;

    // These cameras have electronic shutters.
    if(detector_id == 0)
      mCapabilities.push_back(niad::CAMERA_CAPABILITY_HAS_ELECTRONIC_SHUTTER);
    break;
  default:
    // No default is documented.
    break;
  }
}

SbigSTCamera::~SbigSTCamera() {
  // NOTE: Do not delete the mSTDevice pointer. It is allocated elsewhere.
}

std::string SbigSTCamera::ToString() {

  std::stringstream ss;

  ss << mName << " ";
  if(mDetectorId == 0)
    ss << "Imaging Sensor ";
  else if(mDetectorId == 1)
    ss << "Tracking Sensor ";
  else if(mDetectorId == 2)
    ss << "External Tracking Sensor ";
  else
    ss << "Unknown sensor ";
  ss << "\n";

  ss << " Max Resolution: " << mPixelCount[0] << "x" << mPixelCount[1] << "\n";

  // print out the readout modes
  ss << " Readout Modes: " << readout_modes_.size() << "\n";
  for(auto it = readout_modes_.begin(); it != readout_modes_.end(); ++it) {
    auto id = it->first;
    auto data = it->second;
    ss << "  "
       << "ModeID: " << SBIGReadoutModeToName(id) << " "
       << "Resolution: " << data.max_width << "x" << data.max_height << "\n";
  }

  return ss.str();
}

image * SbigSTCamera::AcquireImage(ExposureSettings settings) {

  // Indicate we are going to do an exposure.
  do_exposure_ = true;

  // Enforce the detector's known limitations.
  auto binning_mode = settings.binning_mode_;
  auto readout_mode = readout_modes_.at(binning_mode);

  // enforce maximum height.
  if(settings.right_ > readout_mode.max_width)
    settings.right_ = readout_mode.max_width;
  if(settings.bottom_ > readout_mode.max_height)
    settings.bottom_  = readout_mode.max_height;

  // enforce exposure duration
  if (settings.exposure_duration_sec_ < mExposureDurationMin)
    settings.exposure_duration_sec_ = mExposureDurationMin;
  if(settings.exposure_duration_sec_ > mExposureDurationMax)
    settings.exposure_duration_sec_ = mExposureDurationMax;

  // Prohibit a guide camera from altering shutter state if the main
  // camera is exposing.
  if(mDetectorId != 0 && mSTDevice->GetMainCamera()->ImageInProgress())
    settings.shutter_action_ = ExposureSettings::SHUTTER_LEAVE_ALONE;

  // Get the driver
  SbigSTDriver &drv = SbigSTDriver::GetInstance();

  // unpack things from the settings
  uint16_t exposure_time_csec = settings.exposure_duration_sec_ * 100;
  uint16_t top                = settings.top_;
  uint16_t left               = settings.left_;
  uint16_t width              = settings.right_ - settings.left_;
  uint16_t height             = settings.bottom_ - settings.top_;
  uint16_t bin_mode           = settings.binning_mode_;
  uint16_t shutter_state      = (uint16_t) settings.shutter_action_;

  // start the exposure
  std::cout << mDetectorId << " Taking exposure of "
            << exposure_time_csec * 10 << " ms long." << std::endl;

  // start the exposure
  StartExposureParams2 se_p;
  se_p.ccd = mDetectorId;
  se_p.exposureTime = exposure_time_csec;
  se_p.abgState = 0;
  se_p.openShutter = shutter_state;
  se_p.readoutMode = bin_mode;
  se_p.top = top;
  se_p.left = left;
  se_p.height = height;
  se_p.width = width;
  drv.RunCommand(CC_START_EXPOSURE2, &se_p, nullptr, mSTDevice->GetHandle());

  // record the start of the exposure
  auto exposure_start = std::chrono::high_resolution_clock::now();

  // Instruct the thread to sleep for most of the exposure, waking
  // up every 100 ms to determine if the exposure should continue.
  // If the exposure is aborted, return a 1x1 pixel image explicitly
  // marked as aborted.
  size_t sleep_intervals = exposure_time_csec * 10.0 / 100;
  for(size_t i = 0; i < sleep_intervals - 1; i++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // return a 1x1 pixel image explicitly labeled as aborted.
    if(!do_exposure_) {
      image * tmp = new image(1,1);
      tmp->aborted_ = true;
      return tmp;
    }
  }

  // For the remainder of the exposure, poll the completion status flag
  // to get an accurate end time. Note that if the driver is busy, the end
  // time can be somewhat inaccurate.
  bool exposure_complete = false;
  QueryCommandStatusParams query_p;
  query_p.command = CC_START_EXPOSURE2;
  QueryCommandStatusResults query_r;
  do {
    drv.RunCommand(CC_QUERY_COMMAND_STATUS, &query_p, &query_r,
                   mSTDevice->GetHandle());

    if (mDetectorId== 0) // main camera
      exposure_complete = get_bit(query_r.status, 0) & get_bit(query_r.status, 1);
    else if (mDetectorId== 1 || mDetectorId== 2) // guide cameras
      exposure_complete = get_bit(query_r.status, 2) & get_bit(query_r.status, 3);

  } while (exposure_complete == false);

  auto exposure_end = std::chrono::high_resolution_clock::now();

  // end the exposure
  EndExposureParams ee_p;
  ee_p.ccd = mDetectorId;
  drv.RunCommand(CC_END_EXPOSURE, &ee_p, nullptr, mSTDevice->GetHandle());

  auto exposure_duration = exposure_end - exposure_start;
  std::cout << mDetectorId << " "
            << "Image took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   exposure_duration)
                   .count()
            << " ms" << std::endl;

  // read the data from the detector
  auto readout_start = std::chrono::high_resolution_clock::now();
  image * img = drv.DoReadout(mSTDevice->GetHandle(), mDetectorId, bin_mode, top,
                           left, width, height);
  auto readout_end = std::chrono::high_resolution_clock::now();

  // TODO: Temporary output for read time
  auto duration = readout_end - readout_start;
  std::cout
      << mDetectorId << " "
      << "Read took "
      << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
      << " ms" << std::endl;

  // set values in the image
  img->exposure_duration_sec_ = settings.exposure_duration_sec_;
  img->exposure_start_        = exposure_start;
  img->exposure_end_          = exposure_end;
  img->filter_name_           = mSTDevice->GetFilterWheel()->GetActiveFilterName();
  img->detector_name_         = mSTDevice->GetInfo().GetDeviceName();

  // Exposure is complete. Reset the flag and return the image.
  do_exposure_ = false;
  return img;
}

void SbigSTCamera::AbortImage() {
  do_exposure_ = false;
}

std::vector<SBIGReadoutMode> SbigSTCamera::GetReadoutModes() {
  std::vector<SBIGReadoutMode> modes;

  for(auto it: readout_modes_) {
    modes.push_back(it.second);
  }

  return modes;
}
