# Mediatek MT6771 Wifi outside of Android

## How to setup and run
Firstly, go into the openmttools-source directory and run build.sh to build the required executables. <br>
Note: This is a modified version of the OpenMTTools Project (https://gitlab.com/Dahrkael/openmttools) modified to support the MT6771. <br>
After that has finished building, copy the 2 built executables (mtinit and mtdaemon) to the openmttools folder. <br>
Then simply run the start-wifi.sh script. <br>

## What about the other parts of the radio?
Note: The ko driver files are not supplied for these due to their non-functional state. <br>
FM Radio --> Appears at /dev/fm but I have no idea how to drive it. <br>
GPS --> Appears at /dev/stpgps, but /dev/gps is missing. More reverse engineering needed. <br>
Bluetooth --> Appears at /dev/stpbt and /dev/btif but is only compatible with Bluedroid, not Bluez. Kind of pointless here. <br>

## Example Output
![alt text](https://github.com/goldenkrew3000/OppoA91/blob/main/Software%20Development/wifi/Example%20Output/image1.png?raw=true)
![alt text](https://github.com/goldenkrew3000/OppoA91/blob/main/Software%20Development/wifi/Example%20Output/image2.png?raw=true)

## Issues
- wmt_drv.ko cannot find nvram. <br>
- Cannot connect to 5GHz networks (Most likely due to the nvram issue) <br>

## Massive thanks to Dahrkael on Gitlab for developing OpenMTTools and PostmarketOS for their info section on getting other Mediatek Radio ICs running in an unsupported environment. This would not have been possible without yall.
