# Install script for directory: /home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/analysisPlugin

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins" TYPE SHARED_LIBRARY FILES "/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/analysisPlugin/build/analysisPlugin.so")
  if(EXISTS "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so"
         OLD_RPATH "/usr/._local/7opty5563ozdhubuhru7wi6wqmmztp7s/lib:/usr/._local/7opty5563ozdhubuhru7wi6wqmmztp7s/lib/root:/opt/software/linux-debian12-x86_64_v2/gcc-12.2.0/root-6.28.04-vluy26n4hk7btjvfbxamqtosp6cdwtsu/lib/root:/usr/local/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/EICrecon_MY/plugins/analysisPlugin.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/belle2/anatochi/geant4/geant4-workdir/EIC_SR_Geant4/eic_reco/analysisPlugin/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
