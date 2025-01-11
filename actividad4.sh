#!/bin/bash


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
DIRECTORY_PATH="~/robocomp/components/grupo4/practica4/Activity_3"
session_id=$(qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.addSession)
qdbus org.kde.yakuake /yakuake/tabs org.kde.yakuake.setTabTitle "$session_id" "$TAB_NAME"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cd $DIRECTORY_PATH"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "cmake . && make -j8"
qdbus org.kde.yakuake /yakuake/sessions runCommandInTerminal $session_id "bin/obs_person_tracker etc/config"
qdbus org.kde.yakuake /yakuake/sessions org.kde.yakuake.raiseSession $session_id