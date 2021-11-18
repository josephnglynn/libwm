#!/bin/bash
Xephyr -br -ac -noreset -screen 800x600 :1 &
sleep 1
DISPLAY=:1 "./build/src/main" &