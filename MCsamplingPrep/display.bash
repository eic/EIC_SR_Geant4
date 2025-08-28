#!/bin/bash
#source /home/belle2/anatochi/eic/epic/install/setup.sh
#source /opt/detector/epic-main/bin/thisepic.sh
# dd_web_display --export $DETECTOR_PATH/$DETECTOR_CONFIG.xml

#source /Users/andriinatochii/eic/epic/install/bin/thisepic.sh
#dd_web_display --export /Users/andriinatochii/eic/epic/install/share/epic/epic_forward_detectors.xml

#source /Users/andriinatochii/eic/epic/install/bin/thisepic.sh; 
#dd_web_display --export /Users/andriinatochii/eic/epic/install/share/epic/epic_ip6_extended.xml

source /Users/andriinatochii/eic/epic/install/bin/thisepic.sh; 
#dd_web_display --export ./epic_cut_main.xml
#dd_web_display --export ./epic_cut_25.01.1.xml
dd_web_display --export ./epic_ip6_cut_25.01.1.xml
