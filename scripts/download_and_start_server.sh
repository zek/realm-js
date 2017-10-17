#!/bin/bash

set -o pipefail
set -e
echo "Downloading and starting ROS. Current directory: " $(pwd)
sh scripts/download-object-server.sh && export ROS_TOS_EMAIL_ADDRESS="realm-js@realm.io" && export DOCKER_DATA_PATH="." && ./node_modules/.bin/ros start --data realm-object-server-data && echo \"Server PID: $!\"