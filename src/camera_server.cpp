
// project includes
#include "device-enums.pb.h"
#include "niad-tools.hpp"
#include "version.h"

// local includes

// vendor-specific includes
#include "sbig_st_driver.hpp"
#include "sbig_st_device.hpp"
#include "sbig_st_camera.hpp"

// system includes
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QTimeZone>
#include <QWebSocket>
#include <QCommandLineParser>
#include <QCommandLineOption>

/// Default Qt Message handler
QtMessageHandler defaultMessageHandler;

int setup_camera(std::shared_ptr<Camera> &main_camera,
                 std::shared_ptr<Camera> &guide_camera,
                 std::shared_ptr<FilterWheel> &filter_wheel);

int main(int argc, char *argv[]) {
  using namespace std;
  using namespace niad;

  qInfo() << "Starting SBIG Camera Application version "
          << NIAD_EXAMPLES_VERSION;

  // Configure the application globally.
  QCoreApplication a(argc, argv);
  QCoreApplication::setOrganizationName("kloppenborg.net");
  QCoreApplication::setOrganizationDomain("kloppenborg.net");
  QCoreApplication::setApplicationName("SBIGCameraApp");

  QCommandLineParser parser;
  parser.setApplicationDescription("Command line control for SBIG camera");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("object", "The object being observed");
  parser.addPositionalArgument("num_exposures", "Total number of exposures");
  parser.addPositionalArgument("exposure_duration",
                               "Exposure duration (seconds)");

  parser.addOptions(
                    {{"temperature", "Set point temperature (Celsius)", "temperature"},
                     {"start", "Start time (ISO 8601 format)", "start"},
                     {"stop", "Stop time (ISO 8601 format)", "stop"},
                     {"filter", "Photometric filter to use", "filter"},
                     {"telescope-uri", "URI to a NIAD telescope", "url"}});

  // Process command line options
  parser.process(a);

  // Get the camera ready
  std::shared_ptr<Camera> main_camera;
  std::shared_ptr<Camera> guide_camera;
  std::shared_ptr<FilterWheel> filter_wheel;
  setup_camera(main_camera, guide_camera, filter_wheel);

  if (main_camera == nullptr || guide_camera == nullptr) {
    cerr << "Could not find specified camera." << endl;
    return -1;
  }

  if (filter_wheel == nullptr) {
    cerr << "Could not find specified filter wheel." << endl;
    return -1;
  }

  // Let the user set the temperature without additional arguments.
  bool set_temperature = false;
  double temperature = 0;
  if (parser.isSet("temperature")) {
    set_temperature = true;
    temperature = parser.value("temperature").toDouble();

    // Set the temperature
    main_camera->setTemperatureTarget(niad::TEMPERATURE_TYPE_SENSOR,
                                      set_temperature, temperature);

    cout << "Set temperature to " << temperature << " C" << endl;
    return 0;
  }

  // Set up the arguments
  auto positionalArguments = parser.positionalArguments();
  QString object_id = positionalArguments[0];
  int num_exposures = positionalArguments[1].toInt();
  double exposure_duration = positionalArguments[2].toDouble();

  if (positionalArguments.isEmpty()) {
    cerr << "Missing required arguments. See -h for more information."
         << endl;
    return -1;
  }

  QDateTime start_time = QDateTime::currentDateTimeUtc();
  if (parser.isSet("start")) {
    start_time = QDateTime::fromString(parser.value("start"));
  }

  QDateTime stop_time = QDateTime::currentDateTimeUtc();
  if (parser.isSet("stop")) {
    stop_time = QDateTime::fromString(parser.value("stop"));
  }

  QString filter = "None";
  bool set_filter = false;
  if (parser.isSet("filter")) {
    set_filter = true;
    filter = parser.value("filter");
  }

  QString telescope_url = "None";
  bool connect_to_telescope = false;
  if (parser.isSet("telescope-url")) {
    telescope_url = parser.value("telescope-url");
  }

  if (set_filter) {
    filter_wheel->setFilter(filter.toStdString());
  }
}

int setup_camera(std::shared_ptr<Camera> & main_camera,
                 std::shared_ptr<Camera> & guide_camera,
                 std::shared_ptr<FilterWheel> & filter_wheel) {

  // Set up the SBIG camera.
  std::string c_name = "ST-10";
  std::string f_name = "CFW-8";
  auto &driver = SbigSTDriver::GetInstance();
  // find a device matching those specifications
  auto info = driver.FindDevice(c_name, f_name);
  if (!info.IsValidDevice()) {
    qInfo() << "Failed to find the specified device.";
    return -1;
  }

  std::string filter_set = "RGBL";
  if (filter_set == "RGBL") {
    info.SetFilter(1, "Red");
    info.SetFilter(2, "Green");
    info.SetFilter(3, "Blue");
    info.SetFilter(4, "Clear");
    info.SetFilter(5, "None");
  } else if (filter_set == "UBVRI") {
    info.SetFilter(1, "U");
    info.SetFilter(2, "B");
    info.SetFilter(3, "V");
    info.SetFilter(4, "R");
    info.SetFilter(5, "I");
  }

  // Get a shared pointer to the cameras and filter wheel.
  auto device = driver.OpenDevice(info);
  main_camera = device->GetMainCamera();
  guide_camera = device->GetGuideCamera();
  filter_wheel = device->GetFilterWheel();

  return 0;
}
