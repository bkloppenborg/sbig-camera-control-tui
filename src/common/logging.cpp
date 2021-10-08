// system includes
#include <iostream>
#include <sstream>
#include <QByteArray>
#include <QStringLiteral>

// project includes

// local includes
#include "logging.hpp"

extern QtMessageHandler defaultMessageHandler;

Q_LOGGING_CATEGORY(commWebSocket, "comm.websocket");

void logMessageHandler(QtMsgType type, const QMessageLogContext &context,
                       const QString &msg) {
  // if(strcmp(context.category, "comm.serial") == 0)
  // logCommSerialHandler(type, context, msg);
  // else
  defaultMessageHandler(type, context, msg);
}

void logCommSerialHandler(QtMsgType type, const QMessageLogContext &context,
                          const QString &msg) {

  QByteArray localMsg = msg.toLocal8Bit();
  switch (type) {
  case QtDebugMsg:
  case QtInfoMsg:
  case QtWarningMsg:
  case QtCriticalMsg:
  case QtFatalMsg:
  default:
    fprintf(stderr, "comm.serial %s\n", localMsg.constData());
    break;
  }
}

// initialize the instance
LogConfiguration *LogConfiguration::mInstance = nullptr;

LogConfiguration::LogConfiguration() {

}

LogConfiguration* LogConfiguration::getInstance() {
  if(mInstance == nullptr)
    mInstance = new LogConfiguration();

  return mInstance;
}

void LogConfiguration::addLogFilterRules(const std::vector<std::string> & rules) {

  for(auto & r: rules)
    mLogRules.push_back(r);
}

std::string LogConfiguration::getLogFilterRules() {
  std::stringstream ss;
  for (auto &r : mLogRules)
    ss << r << "\n";

  return ss.str();
}
