#!/bin/bash
# Farmbot.sh
# This script is designed to be run on a Raspberry Pi with a arduino connected to it.

# check arguments
# -i for installing required packages and setting up environment
# -r for running the program
# -m for running the program in monitor mode
# -h for help
# check if the user is root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# check if script is being run from a folder in root directory
if [ ! -d "/FarmBOT" ]
  then echo "Please move the folder to the root directory"
  exit
fi
# check if the FarmBot folder contains an .env file
if [ ! -f "/FarmBOT/.env" ]
  then echo "Please add the .env file to the FarmBOT folder"
  exit
fi


# check if the user has provided an argument
if [ $# -eq 0 ]
  then echo "No arguments supplied please use -h for help"
  exit
fi

# check if the user has provided the correct argument
if [ $1 != "-i" ] && [ $1 != "-r" ] && [ $1 != "-h" ] && [ $1 != "-m" ]
  then echo "Incorrect argument supplied"
  exit
fi

# display help
if [ $1 == "-h" ]; then
  echo "This script is designed to be run on a Raspberry Pi with a arduino connected to it."
  echo "The script can be run with the following arguments:"
  echo "-i for installing required packages and setting up environment"
  echo "-r for running the program"
  echo "-m for running the program in monitor mode"
  echo "-h for help"
  exit
fi

# install required packages on alpine linux
if [ $1 == "-i" ]; then
  echo "Installing required packages"
  pacman -Syu nodejs npm --noconfirm
  npm install pm2@latest -g
  # setting up environment
  echo "Setting up environment"
  cd NodeAPI/ && npm install && cd ../WebUI && npm install && npm run build || echo "Issue happend durring installation " && exit
  echo "Environment setup complete"
  exit
fi

# run the webui and nodeapi with pm2
if [ $1 == "-r" ]; then
  echo "Running the program"
  cd NodeAPI/ || { echo "Error: NodeAPI folder not found"; exit 1; }
  pm2 start index.js --name "NodeAPI" --watch
  if [ $? -eq 0 ]; then
    echo "NodeAPI started"
  else
    echo "Error starting NodeAPI"
    exit 1
  fi

  cd ../WebUI/build/ || { echo "Error: build folder not found"; exit 1; }
  pm2 start index.js --name "WebUI" --watch
  if [ $? -eq 0 ]; then
    echo "WebUI started"
  else
    echo "Error starting WebUI"
    exit 1
  fi

  pm2 save
  pm2 startup
  echo "Program started, use -m to open the monitor mode"
  exit
fi

# open the pm2 monitor
if [ $1 == "-m" ]; then
  echo "Opening monitor mode"
  pm2 monit
fi


