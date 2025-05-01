# DATAFLUX
DATAFLUX is a simple implementation of a master node and a slave node used for sensor monitoring.
The two(or more) nodes communicate via ESP-NOW, and the master node transmits the data through mqtt to any broker

# Installation
This repo is built using the arduino compilers, or simply the arduino IDE

# Dependencies
ALSMD - https://github.com/PanagiotisPrountzosCS/ALSMD
PubSubClient.h - https://github.com/knolleary/pubsubclient (Can be installed through the arduino IDE library manager)
