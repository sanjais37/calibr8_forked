#!/bin/bash

# Define paths
PATH1="/home/sk63837/calibr8"
PATH2="/home/sk63837/calibr8/flavor/linux-shared/build/calibr8/test/synthetic_calibration_examples/inverse_adjoint_vfm"

# Define commands
COMMAND1="capp rebuild calibr8 -j 4"
COMMAND2="time inverse vfm_notch2D_Asym_small_J2_plane_stress.yaml"

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

# Run the second command in the second path
echo "_______________"
echo "Input file path = $PATH2"
echo "========================"
echo "Running the Calibr8 Code"
echo "========================"
echo " " 
cd "$PATH2" || { echo "Failed to change directory to $PATH2"; exit 1; }
$COMMAND2
echo 
echo "Commands executed successfully."
