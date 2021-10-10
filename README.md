SBIG Camera Control TUI
=====

Provides the ability to control legacy SBIG ST-series and related cameras
from the command line. Specifically, the following features are implemented:

* Control the quantity and duration of exposures.
* Schedule exposures to start and end within specific time intervals.
* Enable, disable, and set temperature regulation
* Change filter in attached filter wheel.
* Connect to [NIAD](https://github.com/bkloppenborg/niad)-enabled telescopes.
* Set object name

Note: Although both the primary and guide cameras are fully functional
within this application, only the primary camera is utilized at present.

# Prerequites

The following items are required

* [SBIG Legacy Driver](https://github.com/bkloppenborg/sbig-legacy-driver)
* C++ compiler
* Qt 5.12 with WebSocket support.

# Building

  cd build
  cmake ..
  make

# Usage

See `camera-server -h` for help.

