#!/bin/bash

DOCKER_BUILDKIT=1 docker build --file web_server.Dockerfile .

docker-compose up
