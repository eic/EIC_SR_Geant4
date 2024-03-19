#!/bin/bash
source /home/belle2/anatochi/eic/epic/install/setup.sh
dd_web_display --export $DETECTOR_PATH/$DETECTOR_CONFIG.xml
ls -lrth
