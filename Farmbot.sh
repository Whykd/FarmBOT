#!/bin/bash
# Farmbot.sh
# This script is designed to be run on a Raspberry Pi with a arduino connected to it.


# **************************************************** #
#                       variables                      #
# **************************************************** #


verbose="true"
repolocation="https://github.com/Whykd/FarmBOT"


# **************************************************** #
#                     prerun checks                    #
# **************************************************** #

# check if the script is being run as root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# check if script is being run from a folder in root directory
if [ ! -d "/FarmBOT" ]
  then echo "Please move the folder to the root directory"
  exit
fi

# check if the user has provided an argument
if [ $# -eq 0 ]
  then echo "No arguments supplied please use -h for help"
  exit
fi
#if [ $2 == "-v" ]; then
#  if [ $3 == "1" ]; then
#    verbose="false"
#    exec 1>/dev/null
#  elif [ $3 == "2" ]; then
#    verbose="false"
#  elif [ $3 == "3" ]; then
#    verbose="true"
#  else
#    echo "Incorrect verbose level supplied"
#    exit
#  fi
#  verbose="true"
#fi

# check if the user has provided the correct argument
if [ $1 != "-i" ] && [ $1 != "-r" ] && [ $1 != "-m" ] && [ $1 != "-u" ] && [ $1 != "-fu" ] && [ $1 != "-h" ]; then
  echo "Incorrect argument supplied"
  exit
fi
if [ $1 != "-su" ] && [ $1 != "-f" ] [ $1 != "-init" ] && [ $1 != "-start" ] && [ $1 != "-monit" ] && [ $1 != "-update" ] && [ $1 != "-fullupdate" ] && [ $1 != "-help" ] && [ $1 != "-setup" ] && [ $1 != "-flash" ]; then
  echo "Incorrect argument supplied"
  exit
fi

# **************************************************** #
#                     "backend"                        #
# **************************************************** #
function setup(){
  pacman -Syu nodejs npm motion git arduino-cli --noconfirm
  npm install pm2@latest -g
  systemctl enable motion
  arduino-cli core install arduino:avr
}

function flash(){
  if [ $verbose == "true" ]; then
    echo "Flashing arduino"
  fi
  arduino-cli upgrade
  arduino-cli cache clean
  if [ $verbose == "true" ]; then
    echo "Compiling and uploading arduino code"
  fi
  arduino-cli compile -b arduino:avr:uno /FarmBOT/Code/FarmingBot
  arduino-cli upload /FarmBOT/Code/FarmingBot
}


function start(){
  cd /FarmBOT/NodeAPI/ || { echo "Error: NodeAPI folder not found"; exit 1; }
  pm2 start index.js --name "NodeAPI" --watch
  if [ $? -eq 0 ]; then
    if [ $verbose == "true" ]; then
      echo "NodeAPI started"
    fi
  else
    echo "Error starting NodeAPI"
    exit 1
  fi

  cd /FarmBOT/WebUI/build/ || { echo "Error: build folder not found"; exit 1; }
  pm2 start index.js --name "WebUI" --watch
  if [ $? -eq 0 ];then
    if [ $verbose == "true" ];then
      echo "WebUI started"
    fi
  else
    echo "Error starting WebUI"
    exit 1
  fi

  pm2 save
  pm2 startup
}

function init() {
  if [ $verbose == "true" ]; then
    echo "Setting up environment"
  fi
  cd /FarmBOT/NodeAPI/ && npm install && cd /FarmBOT/WebUI && npm install && npm run build || echo "Issue happend durring installation " && exit
  if [ $verbose == "true" ]; then
    echo "Environment setup complete"
  fi
}
function update() {
  mkdir -p /tmp/FarmBOT
  cp /FarmBOT/NodeAPI/.env /tmp/FarmBOT/.env
  rm -rf /FarmBOT
  git clone $repolocation /FarmBOT
  cp /tmp/FarmBOT/.env /FarmBOT/NodeAPI/.env
  rm -rf /tmp/FarmBOT
  if [ $verbose == "true" ]; then
    echo "Script updated"
  fi
}

function fullupdate() {
  update
  if [ $verbose == "true" ]; then
    echo "Redeploying"
  fi
  init
  start
}

function help(){
  echo "This script is designed to be run on a Raspberry Pi with a arduino connected to it."
  echo "The script can be run with the following arguments:"
  echo "-i  | -init         for installing required packages and setting up environment"
  echo "-r  | -start        for running the program"
  echo "-m  | -monit        for running the program in monitor mode"
  echo "-u  | -update       for updating the script"
  echo "-fu | -fullupdate   for updating the script and redeploying"
  echo "-h  | -help         for help"
  echo "-v as the second arg followed by the verbose level (1-3) for verbose output"
  echo "    1 = none 2 = errors and warnings 3 = errors, warnings and info (default))"
  echo
  echo "    example: ./Farmbot.sh -i -vt 3"
}

# **************************************************** #
#                     "frontend"                       #
# **************************************************** #


# display help
if [[ $1 == "-help" ]] || [[ $1 == "-h" ]];then
  help
  exit
fi

if [[ $1 == "-setup" ]] || [[ $1 == "-su" ]];then
  setup
  exit
fi

# check if the nodeAPI folder contains an .env file
if [ ! -f "/FarmBOT/NodeAPI/.env" ]
  then echo "Please add the .env file to the NodeAPI folder"
  exit
fi

# install required packages on alpine linux
if [[ $1 == "-init" ]] || [[ $1 == "-i" ]];then
  echo "Installing required packages"
  init
  exit
fi



if [[ $1 == "-flash" ]] || [[ $1 == "-f" ]];then
  flash
  exit
fi

# run the webui and nodeapi with pm2
if [[ $1 == "-start" ]] || [[ $1 == "-r" ]];then
  echo "Running the program"
  start
  exit
fi

# open the pm2 monitor
if [[ $1 == "-monit" ]] || [[ $1 == "-m" ]];then
  echo "Opening monitor mode"
  pm2 monit
fi

# updates the script from github by saving the .env file in the NodeAPI folder into a temp folder, delteing the Farmbot folder, pulling the latest version from github and then moving the .env file back into the NodeAPI folder

if [[ $1 == "-update" ]] || [[ $1 == "u" ]];then
  echo "Updating script"
  update
  echo "Script updated"
  exit
fi

if [[ $1 == "-fullupdate" ]] || [[ $1 == "-fu" ]];then
  echo "Updating script"
  fullupdate
  echo "Script updated"
  exit
fi