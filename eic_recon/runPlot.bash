#!/bin/bash

dir='/group/belle2/TMP/anatochi/eic'

for id in 0 1 2 3
do
	time root -l -b -q plot.C\(\"${dir}\",\"output_eicrecon_${id}_hist_all.root\"\)
done
ls -lrth
