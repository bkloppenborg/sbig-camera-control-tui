#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "niad.pb.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QWebSocket>

class Client : public QObject {
  Q_OBJECT;

public:
  Client();
  ~Client();

protected:
  QWebSocket mWebSocket;

  bool mMountIsReady = false;
  niad::Coordinates mMountCoordinates;

  std::atomic<bool> mBufferValues;

  niad::Coordinates mMountLLA;

  niad::Coordinates mLastCoordinates;
  std::vector<niad::Coordinates> mCoordinateBuffer;

protected:
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
  void open(const QString & url);

  void startBuffering();
  void stopBuffering();
  void clearBuffers();

  std::vector<niad::Coordinates> getCoordinates();

  niad::Coordinates getLLA();


  //
}; // Client

#endif // CLIENT_HPP
