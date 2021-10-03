#!/bin/bash

snap install docker
snap start docker
sudo mkdir -p /usr/share/elasticsearch/data

pip3 install --user elasticsearch flask waitress
