#!/bin/bash

# Define paths
PATH1="/home/sk63837/calibr8"

# Define commands
COMMAND1="capp rebuild calibr8 -j 4"

# Run the first command in the first path
echo "Running command 1 in $PATH1"
cd "$PATH1" || { echo "Failed to change directory to $PATH1"; exit 1; }
source capp-setup.sh
source env/linux-shared.sh
capp load
$COMMAND1

# Check if the first command succeeded
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    read -p "Do you want to run the code from the previously compiled state? (yes/no): " choice
    if [[ "$choice" != "yes" && "$choice" != "y" ]]; then
        echo "Terminating the script."
        exit 1
    fi
else
    echo "Compilation succeeded."
fi
