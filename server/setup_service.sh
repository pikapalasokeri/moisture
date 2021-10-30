#!/bin/bash

# User and shit.
sudo adduser --disabled-password --disabled-login --gecos "" moisture_man
sudo groupadd docker
sudo usermod -aG docker moisture_man

# Paths and shit.
# Absolute path to this script. /home/user/bar/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bar
SCRIPTPATH=`dirname $SCRIPT`

# Service and shit.
sudo systemctl stop moisture_server
sudo rm -rf /home/moisture_man/moisture_server
sudo cp -r $SCRIPTPATH /home/moisture_man/moisture_server
sudo chown -R moisture_man:moisture_man /home/moisture_man/moisture_server
sudo cp /home/moisture_man/moisture_server/moisture_server.service /etc/systemd/system/

sudo systemctl daemon-reload
sudo systemctl enable moisture_server
sudo systemctl start moisture_server
