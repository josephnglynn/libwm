#!/bin/bash
[ ! - d "src" ] && echo "ERROR, src folder does not exist, try calling from root" && cd ..
cp config/default_config.json build/src/config.json