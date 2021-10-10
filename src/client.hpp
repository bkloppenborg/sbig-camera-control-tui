#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "niad.pb.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QWebSocket>

/// Class to serve as a client to NIAD devices.
class Client : public QObject {
  Q_OBJECT;

public:
  /// Default constructor
  Client();
  /// Default destructor.
  ~Client();

protected:
  QWebSocket mWebSocket; ///< Websocket connection.

  //
  // Things to control buffering.

  /// Boolean to instruct this class to buffer data
  std::atomic<bool> mBufferValues;

  //
  // Things specific to the interface with a mount.
  //
  bool mMountIsReady = false; ///< Indicates whether or not the mount is ready for use.
  niad::Coordinates mMountLLA; ///< Latitude, Longitude, and Altitude of the mount.
  niad::Coordinates mLastCoordinates; ///< Last coordinates provided by the mount.
  std::vector<niad::Coordinates> mCoordinateBuffer; ///< Buffer of mount coordinates.

protected:
  /// Send a NIAD command
  /// \param e NIAD envelope.
  void send(niad::Envelope e);

public slots:
  /// Slot to handle WebSocket connect events
  void onWebSocketConnect();
  /// Slot to handle WebSocket disconnect events
  void onWebSocketDisconnect();

  /// Slot to process WebSocket text messages
  void processTextMessage(QString message);
  /// Slot to process WebSocket binary messages
  void processBinaryMessage(QByteArray message);

  /// Function to process DeviceEnvelope messages
  void processDeviceEnvelope(const niad::DeviceEnvelope &e);
  /// Function to process MountEnvelope messsages
  void processMountEnvelope(const niad::MountEnvelope &e);

public:
  /// Open a connection to the specified URL.
  /// \param url A valid URL to a NIAD server.
  void open(const QString & url);

  /// Instruct the class to start buffering data.
  void startBuffering();

  /// Instruct the class to stop buffering data.
  void stopBuffering();

  /// Get all mount coordinates buffered by the class.
  std::vector<niad::Coordinates> getCoordinates();

  /// Get the Latitude, Longitude, and Altitude of the mount.
  niad::Coordinates getLLA();

  //
}; // Client

#endif // CLIENT_HPP
