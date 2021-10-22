
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
#include <csignal>

QThread * worker_thread = nullptr;
Worker * worker = nullptr;

void signal_handler(int s) {
  std::signal(s, SIG_DFL);

  qInfo() << "Quitting...";

  worker->stopExposures();
}

int main(int argc, char *argv[]) {
  using namespace std;
  using namespace niad;

  qInfo() << "Starting SBIG Camera Application version "
          << NIAD_EXAMPLES_VERSION
          << "at"
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
  parser.addPositionalArgument("object", "The object being observed");
  parser.addPositionalArgument("exposure_quantity", "Total number of exposures");
  parser.addPositionalArgument("exposure_duration",
                               "Exposure duration (seconds)");

  parser.addOptions({
    {"temperature",
     "Active cooling set point (Celsius). Values > 40 disables cooling.",
     "temperature"},
    {"save-dir",
     "Directory in which the images will be saved (defaults to /tmp)",
     "dir"},
    {"filter",
     "Photometric filter to use",
     "filter"},
    {"telescope-url",
     "URI to a NIAD telescope",
     "url"},
    {"readout-mode",
     "Readout mode to use. Valid options are 1x1, 2x2, 3x3, 9x9",
     "mode"},
    });

  // Process command line options
  parser.process(app);
  auto num_args = parser.positionalArguments().size();

  // Activate the NIAD client. Give it time to connect to NIAD servers
  // before we start taking images.
  Client client;
  QString telescope_url = "None";
  bool connect_to_telescope = false;
  if (parser.isSet("telescope-url")) {
    client.open(parser.value("telescope-url"));
  }

  // Create a camera controler.
  worker_thread = new QThread();
  worker = new Worker(&client);
  worker->moveToThread(worker_thread);
  QObject::connect(worker_thread, &QThread::started, worker, &Worker::run);
  QObject::connect(worker, &Worker::finished, worker_thread, &QThread::quit);
  QObject::connect(worker_thread, &QThread::finished, &app, &QCoreApplication::quit);

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
  QString object_id = positionalArguments[0];
  int num_exposures = positionalArguments[1].toInt();
  double exposure_duration = positionalArguments[2].toDouble();

  worker->setObjectName(positionalArguments[0]);
  worker->setExposureQuantity(positionalArguments[1].toInt());
  worker->setExposureDuration(positionalArguments[2].toDouble());

  if (positionalArguments.isEmpty()) {
    cerr << "Missing required arguments. See -h for more information."
         << endl;
    return -1;
  }

  // Set the save directory.
  if(parser.isSet("save-dir")) {
    worker->setSaveDir(parser.value("save-dir"));
  }

  // Choose the filter to use.
  if (parser.isSet("filter")) {
    worker->setFilter(parser.value("filter"));
  }

  // Set the readout mode.
  niad::CameraReadoutMode readout_mode = niad::CAMERA_READOUT_MODE_1X1;
  if(parser.isSet("readout-mode")) {
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

  // Start taking images.
  worker_thread->start();

  // Run the application and event loop.
  return app.exec();
}

