#!/bin/bash

# WEBOTS
TAB_NAME="webots"
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "rcnode"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia webots"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id


# BRIDGE
TAB_NAME="bridge"
DIRECTORY_PATH="/home/robocomp/robocomp/components/webots-bridge" # replace with your desired path
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/Webots2Robocomp etc/config"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id


# CAMERA
TAB_NAME="camera"
DIRECTORY_PATH="~/robocomp/components/robocomp-robolab/components/hardware/camera/ricoh_omni" # replace with your desired path
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/RicohOmni etc/config_wb"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id

TAB_NAME="rgbd"
DIRECTORY_PATH="~/robocomp/components/robocomp-shadow/insect/RGBD_360" # replace with your desired path
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/RGBD_360 etc/config_wb"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id


TAB_NAME="yolo"
DIRECTORY_PATH="~/robocomp/components/robocomp-shadow/insect/environment_object_perception"
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "src/environment_object_perception.py etc/config_wb"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id


# Grid2D
TAB_NAME="grid2d"
DIRECTORY_PATH="~/robocomp/components/grupo4/practica4/grid2D" 
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/grid2D etc/config"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id


# Obs_person_tracker
TAB_NAME="obs_person_tracker"
DIRECTORY_PATH="~/robocomp/components/grupo4/practica4/Activity3"
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/obs_person_tracker etc/config"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id