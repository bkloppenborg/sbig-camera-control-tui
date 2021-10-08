#ifndef LOGGING_H
#define LOGGING_H

// system includes
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(commWebSocket); // comm.webscoket


void logMessageHandler(QtMsgType type, const QMessageLogContext &context,
                       const QString &msg);

void logCommSerialHandler(QtMsgType type, const QMessageLogContext &context,
                          const QString &msg);

/// Singleton class for storing log configuration data
class LogConfiguration {

private:
  static LogConfiguration * mInstance;
  /// default constructor (private)
  LogConfiguration();

protected:
  /// Current log rules
  std::vector<std::string> mLogRules;

public:
  /// Returns the instance to the LogConfiguration
  static LogConfiguration * getInstance();

public:
  /// Adds the specified rules to the log filter.
  void addLogFilterRules(const std::vector<std::string> & rules);
  /// Returns a string containing all log filter rules
  std::string getLogFilterRules();

}; // class LogConfiguration

#endif // LOGGING_H
