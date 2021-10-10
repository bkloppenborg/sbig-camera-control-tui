#include "client.hpp"

Client::Client() {
  mBufferValues = false;
}

Client::~Client() {

}

void Client::open(const QString & url) {

  mWebSocket.open(url);

  // connect signals and slots
  connect(&mWebSocket, &QWebSocket::connected,
          this, &Client::onWebSocketConnect);
  connect(&mWebSocket, &QWebSocket::disconnected,
          this, &Client::onWebSocketDisconnect);
  connect(&mWebSocket, &QWebSocket::textMessageReceived,
          this, &Client::processTextMessage);
  connect(&mWebSocket, &QWebSocket::binaryMessageReceived,
          this, &Client::processBinaryMessage);
}

void Client::send(niad::Envelope e) {
  QByteArray data(e.SerializeAsString().c_str(), e.ByteSize());
  mWebSocket.sendBinaryMessage(data);
}

void Client::onWebSocketConnect(){
  qDebug() << "WebSocket Connected";
}

void Client::onWebSocketDisconnect(){
  qDebug() << "WebSocket Disconnected";
}

void Client::processTextMessage(QString message){
  // do nothing
}

void Client::processBinaryMessage(QByteArray message){
  using namespace niad;

  Envelope e;
  e.ParseFromArray(message.data(), message.size());

  if (e.has_device_envelope()) {
    processDeviceEnvelope(e.device_envelope());
  }

  if (e.has_mount_envelope()) {
    processMountEnvelope(e.mount_envelope());
  }
}

void Client::processDeviceEnvelope(const niad::DeviceEnvelope &e){
  using namespace niad;

  // Find the first mount and get additional information about it
  for(size_t i = 0; i < e.info_size(); i++) {
    auto info = e.info(i);

    if (info.type() == DEVICE_TYPE_MOUNT) {
      Envelope e;
      MountEnvelope *m_e = e.mutable_mount_envelope();
      m_e->set_type(MOUNT_REQUEST_GET_INFO);
      emit send(e);
    }
  }
}

void Client::processMountEnvelope(const niad::MountEnvelope &m_e){
  using namespace niad;

  if(m_e.type() == MOUNT_RESPONSE_INFO && m_e.has_info()) {
    // indicate the mount is NOT ready.
    mMountIsReady = false;

    // On connect, we should get an info object describing the
    // capabilities of the mount. Start by requesting a subscription
    // to all coordinate systems supported.
    auto info = m_e.info();

    Envelope r;
    auto r_m = r.mutable_mount_envelope();
    for (auto &c : info.capabilities()) {

      // subscribe to relevant coordinate updates
      if (c == MOUNT_CAPABILITY_GET_COORDS_AZM_ALT) {
        r_m->set_type(MOUNT_REQUEST_SUBSCRIBE_COORDS_AZM_ALT);
        emit send(r);
      } else if (c == MOUNT_CAPABILITY_GET_COORDS_RA_DEC) {
        r_m->set_type(MOUNT_REQUEST_SUBSCRIBE_COORDS_RA_DEC);
        emit send(r);
      } else if (c == MOUNT_CAPABILITY_GET_LAT_LON_ALT) {
        r_m->set_type(MOUNT_REQUEST_GET_LAT_LON_ALT);
        emit send(r);
      }
    }

    // Also ask if the mount is ready to go.
    r_m->set_type(MOUNT_REQUEST_GET_IS_READY);
    emit send(r);

  } else if (m_e.has_coords()) {

    auto coords = m_e.coords();

    if(coords.type() == niad::COORDINATE_TYPE_LAT_LON_ALT) {
      mMountLLA = coords;
    } else {

      // put the coordinates into the buffer, if instructed.
      if (mBufferValues)
        mCoordinateBuffer.push_back(coords);

      // Update the latest coordinates.
      mLastCoordinates = coords;
    }

  } else if(m_e.type() == MOUNT_RESPONSE_IS_READY) {
    // indicate the mount is operational.
    mMountIsReady = true;

    // Shut off non RA/DEC coordinate subscriptions.
    Envelope r;
    auto r_m = r.mutable_mount_envelope();
    r_m->set_type(MOUNT_REQUEST_UNSUBSCRIBE_COORDS_AZM_ALT);
    emit send(r);

  } else if (m_e.type() == MOUNT_RESPONSE_IS_NOT_READY) {
    mMountIsReady = false;
  }
}

void Client::startBuffering() {
  mCoordinateBuffer.clear();
  mBufferValues = true;
}

void Client::stopBuffering() {
  mBufferValues = false;
}

std::vector<niad::Coordinates> Client::getCoordinates() {
  return mCoordinateBuffer;
}

niad::Coordinates Client::getLLA() {
  return mMountLLA;
}
