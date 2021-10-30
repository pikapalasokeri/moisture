#!/bin/bash

# Libs and shit.
sudo snap install docker
sudo snap start docker
sudo mkdir -p /usr/share/elasticsearch/data

sudo apt install python3-pip
pip3 install --user elasticsearch flask waitress
