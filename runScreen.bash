#!/bin/bash

LANG=en_US
LC_NUMERIC=en_US.UTF-8
echo ""
echo "WELLCOME, GREAT SCIENTIST!"
echo ""
echo "I'm BiSiCo (Big Simulation Code). I was created by Andrii. My goal is to make your life simpler."
echo ""
date;
echo ""

g4conf; g4cmake; make -j4;

echo -ne "Total number of threads on the machine is: ";
sysctl -n hw.ncpu

chmod u+x runAll.bash;
chmod a+x runAll.bash;

if [ -d macFiles ];
then
        echo "The directory macFiles is already exist.";
else
        mkdir macFiles;
fi

for ThreadID in $(seq 1 1 10)
do
        echo "GNU Screen session name: ThreadID_$ThreadID";

        screen -S THREADID_$ThreadID -d -m ./runAll.bash $ThreadID
done

echo ""
echo "That is all. If you want to follow My processing, type in the terminal 'screen -ls'. And you will see the list of GNU Screen sessions (My thoughts).";
echo "Choose an item and write in the terminal again: 'screen -r SESSION_NAME'. Before using GNU Screen, please, read its help: 'screen --help'.";
echo ""
echo "Enjoy your time. I will do everything automatically. Just don't forget about me :)";
echo ""
date;
echo "";
# ls -lrt output_*.root | awk '{ print $9 }' > mergeFiles.bash
