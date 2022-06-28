#!/bin/bash

export PATH="${PATH}:/snap/bin"

DOCKER_BUILDKIT=1 docker build --file web_server.Dockerfile --tag web_server .
DOCKER_BUILDKIT=1 docker build --file data_receiver.Dockerfile --tag data_receiver .

docker-compose up
