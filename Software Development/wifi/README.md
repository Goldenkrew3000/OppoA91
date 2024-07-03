# Mediatek MT6771 Wifi outside of Android

## How to setup and run
### Setup the android-like environment to keep the drivers happy
Firstly, extract your 'system' and 'vendor' partitions using MTKClient, and extract the 2 images (I used 7zip) into folders. <br>
In the root of your filesystem, make the following folder structure: <br>
/system <br>
/system/bin <br>
/system/lib <br>
/system/lib64 <br>
/apex/com.android.runtime/bin <br>
/data/misc/firmware/active <br>
Okay, now copy 'linker' and 'linker64' from your 'extracted-system/system/bin/' to '/system/bin/'. Chmod +x both of those. <br>
Now ln -s both of them to '/apex/com.android.runtime/bin/'. <br>
Now copy everything from your 'extracted-system/system/lib/' to '/system/lib/'. <br>
Now copy everything from your 'extracted-system/system/lib64/' to '/system/lib64/'. <br>
Now copy everything from your 'extracted-vendor/firmware/' to '/lib/firmware/' and '/data/misc/firmware/active/'. <br>
The 2 firmware folders basically have to be a direct copy of each other. <br>
Now I know not all of these files are needed, but some are, and this is a really good base for any other stuff happening in this repository. <br>
Note: I have provided my own /system and /data in 'base-system-files.tar.bz2' in case something does not work and you and you need to compare the files. <br>

### Actually build and run the software
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
