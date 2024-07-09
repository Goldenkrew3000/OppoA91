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
Now you have to edit ```/etc/inittab``` to remove all ```tty1-6``` terminals and modify the ```ttyS0``` line to read ```ttyS0::respawn:/sbin/getty -L 921600 ttyS0 vt100``` <br>

