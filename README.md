NIAD Mount Server
=====

A server providing Network Interface for Astrophysical Devices (NIAD) 
capabilities to legacy telescope mounts.

# Prerequites

The following items are required

* C++ compiler
* Qt 5.12 with WebSockets and SerialPort packages

# Building

  cd build
  cmake ..
  make

# Configuration Options

The server uses `QSettings` for storing configuration options. See
[QSettings documentation](https://doc.qt.io/qt-5/qsettings.html#locations-where-application-settings-are-stored) 
for location of configuration options on various platforms.

# Advanced Logging

There are a variety of advanced logging options in the mount server.
To enable all of them, run the following command:

    QT_LOGGING_RULES="*.debug=true" ./src/mount-server
    
# Supported Devices

## Mounts

At present this software supports:

*Celestron Mounts including*

GPS Series, i-Series, i-Series SE, CGE, Advanced GT, SLT, CPC ,GT, NexStar 4/5 SE
NexStar 6/8 SE, CGE Pro, CGEM DX, LCM, Sky Prodigy, CPC Deluxe, GT 16, StarSeeker
AVX, Cosmos, Evolution, CGX, CGXL, Astrofi, SkyWatcher
