#!/bin/bash

# Start the first process
./mosquitto -D
status=$?
if [ $status -ne 0 ]; then
  echo "Failed to start mosquitto: $status"
  exit $status
fi

./ffmpeg 156.38.174.66 1883 127.0.0.1 1883



