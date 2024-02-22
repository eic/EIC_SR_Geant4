#!/bin/bash

#--------------------------------------------------#
#---------------- INPUT PARAMETERS ----------------#
inputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicsim_synrad_new_sampling_highstat' # input directory
outputDirName='/gpfs/group/belle2/TMP/anatochi/eic/output_eicrecon_synrad_new_sampling_highstat' # output directory
buildDir='./build_new_sampling_highstat' # build directory
queue='hx'
eicshell='/home/belle2/anatochi/eic/eic-shell'
# eicsetup='/opt/detector/setup.sh'
eicsetup='/home/belle2/anatochi/eic/epic/install/setup.sh'
#--------------------------------------------------#
#--------------------------------------------------#

echo "[INFO] inputDirName = ${inputDirName}"
echo "[INFO] outputDirName = ${outputDirName}"
echo "[INFO] buildDir = ${buildDir}"
echo "[INFO] queue = ${queue}"

#-------------------------------------------------------------#
#---------- Compile the custom analysis plugin ---------------#
#<<COMMENT
	rm -rf analysisPlugin/build
	rm -rf EICrecon_MY
	mkdir -pv EICrecon_MY

	compileScript='compileAnalysisPlugin.bash'
	echo "#!/bin/bash" > ${compileScript}
	echo "source ${eicsetup}" >> ${compileScript}
	echo "export EICrecon_MY=${PWD}/EICrecon_MY" >> ${compileScript}
	echo "cmake -S analysisPlugin -B analysisPlugin/build" >> ${compileScript}
	echo "cmake --build analysisPlugin/build --target install" >> ${compileScript}

	chmod 777 ${compileScript}
	. $eicshell -- ./${compileScript}
	rm -rf ${compileScript}
#COMMENT
#-------------------------------------------------------------#
#-------------------------------------------------------------#

# output directory
mkdir -pv ${outputDirName}

# build directory
mkdir -pv ${buildDir}
cd ${buildDir}

# submit a job
runFile="runEICrecon.sh"
echo "#!/bin/bash" > ${runFile}
echo "source ${eicsetup}" >> ${runFile}
echo "export EICrecon_MY=${PWD}/../EICrecon_MY" >> ${runFile}
echo -ne "time eicrecon -Pplugins=analysisPlugin -Ppodio:output_include_collections=\"MCParticles,VertexBarrelHits,TrackerEndcapHits,SiBarrelHits\" " >> ${runFile} 
echo -ne "-Ppodio:output_file=${outputDirName}/output_eicrecon.podio.root " >> ${runFile} 
echo "-Phistsfile=${outputDirName}/output_eicrecon.hist.root ${inputDirName}/output_eicsim.edm4hep.root" >> ${runFile}

chmod 777 ${runFile}
bsub -q $queue -o ${outputDirName}/logJob_eicrecon.txt ". ${eicshell} -- ./${runFile}"

cd -

