
// project includes
#include "device-enums.pb.h"
#include "niad-tools.hpp"
#include "version.h"

// local includes
#include "worker.hpp"
#include "client.hpp"

// system includes
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QTimeZone>
#include <QWebSocket>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include <csignal>

QThread * worker_thread = nullptr;
Worker * worker = nullptr;

void signal_handler(int s) {
  std::signal(s, SIG_DFL);

  qInfo() << "Quitting...";

  worker->stopExposures();
}

int setup_from_cli(Worker *worker, QThread *worker_thread,
                   Client &client,
                   QCommandLineParser &parser);

int setup_from_config(Worker *worker, QThread *worker_thread,
                      Client &client,
                      const QString & filename,
                      QCommandLineParser &parser);

int main(int argc, char *argv[]) {
  using namespace std;
  using namespace niad;

  qInfo() << "Starting SBIG Camera Application version "
          << NIAD_EXAMPLES_VERSION << "at"
          << QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

  // Configure the application globally.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("kloppenborg.net");
  QCoreApplication::setOrganizationDomain("kloppenborg.net");
  QCoreApplication::setApplicationName("SBIGCameraApp");

  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  QCommandLineParser parser;
  parser.setApplicationDescription("Command line control for SBIG camera");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("exposure_quantity",
                               "Total number of exposures");
  parser.addPositionalArgument("exposure_duration",
                               "Exposure duration (seconds)");

  parser.addOptions({
      {"config",
       "Configuration file",
       "config"},
      {"catalog",
       "The catalog to which the object belongs",
       "catalog"},
      {"object_id",
       "The identifier in the catalog for the specified object",
       "object_id"},
      {"temperature",
       "Active cooling set point (Celsius). Values > 40 disables cooling.",
       "temperature"},
      {"save-dir",
       "Directory in which the images will be saved (defaults to /tmp)",
       "dir"},
      {"filter", "Photometric filter to use",
       "filter"},
      {"telescope-url", "URI to a NIAD telescope",
       "url"},
      {"readout-mode",
       "Readout mode to use. Valid options are 1x1, 2x2, 3x3, 9x9",
       "mode"},
      {"shutter-mode",
       "Shutter mode to use. Valid options are OPEN_CLOSE [default], "
       "CLOSE_CLOSE",
       "shutter-mode"}});

  // Process command line options
  parser.process(app);
  auto num_args = parser.positionalArguments().size();

  Client client;

  // Create a camera controler.
  worker_thread = new QThread();
  worker = new Worker(&client);
  worker->moveToThread(worker_thread);
  QObject::connect(worker_thread, &QThread::started, worker, &Worker::run);
  QObject::connect(worker, &Worker::finished, worker_thread, &QThread::quit);
  QObject::connect(worker_thread, &QThread::finished, &app,
                   &QCoreApplication::quit);

  // Configure the application from either the CLI or parser.
  int status = 0;
  if (parser.isSet("config")) {
    // Get the filename
    QString filename = parser.value("config");

    // Verify that the file exists.
    QFileInfo cfg_file(filename);
    if(cfg_file.exists() && cfg_file.isFile()) {
      setup_from_config(worker, worker_thread, client, filename, parser);
    } else {
      qWarning() << "Configuration file not found. Exiting.";
      return 0;
    }
  } else {
    setup_from_cli(worker, worker_thread, client, parser);
  }

  // Start taking images.
  worker_thread->start();

  // Run the application and event loop.
  return app.exec();
}

int setup_from_cli(Worker * worker, QThread * worker_thread, Client & client,
                   QCommandLineParser & parser) {
  using namespace std;

  qInfo() << "Loading information from CLI";

  // Determine the number of arguments
  int num_args = parser.positionalArguments().size();

  // Activate the NIAD client. Give it time to connect to NIAD servers
  // before we start taking images.
  QString telescope_url = "None";
  bool connect_to_telescope = false;
  if (parser.isSet("telescope-url")) {
    client.open(parser.value("telescope-url"));
  }

  // Set the temperature. If there are no other requests, exit.
  double temperature = 0;
  if (parser.isSet("temperature")) {
    temperature = parser.value("temperature").toDouble();

    worker->setTemperature(temperature);

    // If there are no further arguments, exit gracefully.
    if (num_args == 0) {
      return 0;
    }
  }

  // Set up the arguments
  auto positionalArguments = parser.positionalArguments();
  int num_exposures = positionalArguments[0].toInt();
  worker->setExposureQuantity(positionalArguments[0].toInt());
  double exposure_duration = positionalArguments[1].toDouble();
  worker->setExposureDuration(positionalArguments[1].toDouble());

  if (positionalArguments.isEmpty()) {
    cerr << "Missing required arguments. See -h for more information."
         << endl;
    return -1;
  }

  // Catalog name
  QString catalog_name = "NONE";
  if(parser.isSet("catalog")) {
    catalog_name = parser.value("catalog");
  }
  worker->setCatalogName(catalog_name);
  qInfo() << "Catalog:" << catalog_name;

  // Object Name
  QString object_id = "NONE";
  if(parser.isSet("object_id")) {
    object_id = parser.value("object_id");
  }
  worker->setObjectName(catalog_name);
  qInfo() << "Object ID:" << object_id;

  // Set the save directory.
  if (parser.isSet("save-dir")) {
    worker->setSaveDir(parser.value("save-dir"));
  }

  // Choose the filter to use.
  if (parser.isSet("filter")) {
    worker->setFilter(parser.value("filter"));
  }

  // Set the readout mode.
  niad::CameraReadoutMode readout_mode = niad::CAMERA_READOUT_MODE_1X1;
  if (parser.isSet("readout-mode")) {
    QString mode = parser.value("readout-mode");
    if (mode == "1x1") {
      readout_mode = niad::CAMERA_READOUT_MODE_1X1;
    } else if (mode == "2x2") {
      readout_mode = niad::CAMERA_READOUT_MODE_2X2;
    } else if (mode == "3x3") {
      readout_mode = niad::CAMERA_READOUT_MODE_3X3;
    } else if (mode == "9x9") {
      readout_mode = niad::CAMERA_READOUT_MODE_9X9;
    } else {
      cerr << "Readout mode '" << mode.toStdString() << "' not supported."
           << endl;
      return -1;
    }
  }
  worker->setReadoutMode(readout_mode);

  niad::CameraShutterAction shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE;
  if(parser.isSet("shutter-mode")) {
    QString mode = parser.value("shutter-mode");
    if(mode == "CLOSE_CLOSE") {
      shutter_action = niad::CAMERA_SHUTTER_ACTION_CLOSE_CLOSE;
    } else if (mode == "OPEN_OPEN") {
      shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_OPEN;
    }
  }
  worker->setShutterAction(shutter_action);

  return 0;
}

int setup_from_config(Worker *worker, QThread *worker_thread,
                      Client &client,
                      const QString &filename,
                      QCommandLineParser &parser) {

  qInfo() << "Loading configuration from file.";

  QSettings settings(filename, QSettings::IniFormat);

  // Activate the NIAD client. Give it time to connect to NIAD servers
  // before we start taking images.
  QString telescope_url = settings.value("mount/url").toString();
  qInfo() << "Mount URL:" << telescope_url;
  client.open(telescope_url);

  // Read the catalog from the configuration file
  QString object_catalog = settings.value("object_info/catalog").toString();
  // Override the catalog name if it was specified on the CLI
  if(parser.isSet("catalog")) {
    object_catalog = parser.value("catalog");
  }
  worker->setCatalogName(object_catalog);
  qInfo() << "Catalog:" << object_catalog;

  // Read the object from the configuration file
  QString object_id = settings.value("object_info/object_id").toString();
  if(parser.isSet("object_id")) {
    object_id = parser.value("object_id");
  }
  worker->setObjectName(object_id);
  qInfo() << "Object ID:" << object_id;

  // Set exposure settings
  int exposure_quantity = settings.value("camera/exposure_quantity").toInt();
  qInfo() << "Exposure Quantity:" << exposure_quantity;
  worker->setExposureQuantity(exposure_quantity);

  double exposure_duration = settings.value("camera/exposure_duration").toDouble();
  qInfo() << "Exposure Duration:" << exposure_duration;
  worker->setExposureDuration(exposure_duration);

  // Set the temperature.
  double temperature = settings.value("camera/temperature").toDouble();
  qInfo() << "Set Temperature:" << temperature;
  worker->setTemperature(temperature);

  // Set up the save directory.
  QString base_dir = settings.value("global/base_dir").toString();
  QString raw_dir = settings.value("global/raw_sub_dir").toString();
  QString save_dir = base_dir + "/" + raw_dir;
  qInfo() << "Save Dir:" << save_dir;
  worker->setSaveDir(save_dir);

  // Set the filter
  QString filter = settings.value("camera/filter").toString();
  qInfo() << "Filter:" << filter;
  worker->setFilter(filter);

  // Set the readout mode.
  niad::CameraReadoutMode readout_mode = niad::CAMERA_READOUT_MODE_1X1;
  QString mode = settings.value("camera/readout_mode").toString();
  qInfo() << "Readout Mode:" << mode;
  if (mode == "1x1") {
    readout_mode = niad::CAMERA_READOUT_MODE_1X1;
  } else if (mode == "2x2") {
    readout_mode = niad::CAMERA_READOUT_MODE_2X2;
  } else if (mode == "3x3") {
    readout_mode = niad::CAMERA_READOUT_MODE_3X3;
  } else if (mode == "9x9") {
    readout_mode = niad::CAMERA_READOUT_MODE_9X9;
  } else {
    std::cerr << "Readout mode '" << mode.toStdString() << "' not supported."
              << std::endl;
    return -1;
  }
  worker->setReadoutMode(readout_mode);

  niad::CameraShutterAction shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_CLOSE;
  QString action = settings.value("camera/shutter_action").toString();
  qInfo() << "Shutter Action:" << action;
  if (action == "CLOSE_CLOSE") {
    shutter_action = niad::CAMERA_SHUTTER_ACTION_CLOSE_CLOSE;
  } else if (action == "OPEN_OPEN") {
    shutter_action = niad::CAMERA_SHUTTER_ACTION_OPEN_OPEN;
  }
  worker->setShutterAction(shutter_action);

  return 0;
}
