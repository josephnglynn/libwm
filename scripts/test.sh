#!/bin/bash
unset XDG_SEAT
Xephyr -br -ac -noreset -screen 1280x1024 :1 &
# -br -ac -noreset -screen 1920x1080 :2 &
#./sleep 2 $(DISPLAY=:1 alacritty)

#sleep 1
#DISPLAY=:1 "./build/src/main" &
