# Running Alpine Linux

## Currently Functional:
* 2.4GHz WiFi
* Touchscreen
* XOrg with FBDev (No hardware acceration)
* Screen Brightness
* Battery monitoring

## Parts Required:
* SD Card (This is your system drive so any size you need)

# Assembling the RootFS
## Step 1 - Format your SD Card
The default initrd boots from the first partition (/dev/mmcblk0p1), so format your SD Card as GPT and make it's first (and only) partition ext4. <br>

## Step 2 - Download the aarch64 mini rootfs of Alpine Linux
This is located at: https://alpinelinux.org/downloads/ <br>

## Step 3 - Extract the rootfs tarball to the root of your SD Card
From this point on, continue as root! <br>
In terminal, ```cd``` to the partition you just created, and run ```tar -xvf <location of the tarball>``` <br>

## Step 4 - Chroot into the filesystem and configure it
Now type ```chroot <location of the filesystem> /bin/sh``` <br>
First, to get internet, you need to configure your ```/etc/resolv.conf``` file: ```echo nameserver <default gateway> > /etc/resolv.conf``` <br>
Now to make the rootfs bootable, run the following commands: <br>
```
apk add alpine-base
rc-update add devfs sysinit
rc-update add dmesg sysinit
rc-update add mdev sysinit
rc-update add sysctl boot
rc-update add hostname boot
rc-update add bootmisc
rc-update add syslog boot
rc-update add networking boot
rc-update add mount-ro shutdown
rc-update add killprocs shutdown
rc-update add savecache shutdown
```
And install ```dhclient``` with ```apk add dhclient``` <br>
You also have to install a text editor (nano / vim): ```apk add vim``` <br>
Now you have to edit ```/etc/inittab``` to remove all ```tty1-6``` terminals and modify the ```ttyS0``` line to be ```ttyS0::respawn:/sbin/getty -L 921600 ttyS0 vt100``` <br>

## Step 5 - USB Networking
To obtain wired networking through a USB Ethernet dongle during boot automatically, add the following to ```/etc/network/interfaces``` <br>
Note: If you do not want USB networking, still add the top 2 lines as this sets up the local loopback <br>
```
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet dhcp
```

## Step 6 - Set a password
Set a password because otherwise you won't be able to login with ```passwd``` <br>
At this point, you can also make a new non-root user with Alpine's ```setup-user``` command. If you do, don't forget to install sudo with ```apk add sudo``` <br>

## Step 7 - Install OpenSSH Server
The device cannot be used through the touchscreen alone (This is super super alpha software) and the UART is kind of dreadful to use for anything past short commands. <br>
Install and configure OpenSSH Server to start on boot with the following commands: <br>
```
apk add openssh
rc-update add sshd default
```
Note: If you did not create a new user (bad security practice ```*bonk*```), don't forget to allow root login at ```/etc/ssh/sshd_config```

## Step 8 - Boot
Note: Go to the ```Initrds``` folder in this repository to find out more info on actually booting the device. <br>

## (Optional) Step 9 - (2.4GHz) WiFi
Follow the instructions in the ```Software Development/Wifi``` folder of this repository. <br> TODO

## (Optional) Step 10 - Copy the rootfs from the SD Card to the internal UFS
Note: This will wipe your ```userdata``` and possibly (Depending on how you setup the initrd) the ```system``` partitions of your device. <br>
Note: Go to the ```Initrds``` folder in this repository to find out more info on actually booting from the UFS. <br>
Note: This guide assumes that your ```userdata``` partition is ```/dev/sdc41``` <br>
Firstly, boot into the SD Card rootfs on the phone


Firstly, boot into the Debug Initrd. When it comes up with the login over UART, just press ```Ctrl + C``` to get to the terminal. <br>
Now setup and mount the SD Card rootfs with the following commands:
```
```
Now mount the ```userdata``` partition of the UFS with the following commands:
``` TODO
```
