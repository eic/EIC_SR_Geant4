#!/bin/bash

directory="/gpfs02/eic/anatochii/output_v6.3_nosrmask"
extension=".root"
outputfile="/gpfs02/eic/anatochii/output_v6.3_nosrmask_tracks.root"
picsdir="pics_v6.3_nosrmask"

mkdir -pv ${picsdir}

time root -l -b -q plotTracks.C\(\"${directory}\",\"${extension}\",\"${outputfile}\",\"${picsdir}\"\)
