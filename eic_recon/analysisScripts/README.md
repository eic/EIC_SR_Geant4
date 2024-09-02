To plot hits per detector:
1. open a screen session:
>> screen -S 18_nosrmask_hits
2. In the open session run:
>> energy='18x275'; mask='nosrmask'; cd ~/; source goToConda.sh; cd -; time root -l -b -q drawAllHits.C\(\"/gpfs02/eic/anatochii/output_eicrecon_v6.3.1_${mask}_${energy}GeV_highStat\",\".hist\",\".root\",\"pics_${energy}GeV_${mask}_ana\"\) &> pics_${energy}GeV_${mask}_ana/log_drawAllHits.txt

To plot vetices per detector:
1. open a screen session:
>> screen -S 18_nosrmask_vtx
2. In the open session run:
>> energy='18x275'; mask='nosrmask'; cd ~/; source goToConda.sh; cd -; time root -l -b -q drawAllVertices.C\(\"/gpfs02/eic/anatochii/output_eicrecon_v6.3.1_${mask}_${energy}GeV_highStat\",\".vertex\",\".root\",\"pics_${energy}GeV_${mask}_ana\"\) &> pics_${energy}GeV_${mask}_ana/log_drawAllVertices.txt
