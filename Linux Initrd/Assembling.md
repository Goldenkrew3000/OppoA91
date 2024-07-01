Okay so the main way I did this was by making a fully custom initrd that the prebuilt kernel loads, and interacting with it over UART.

All it does is some mounting and it uses busybox's 'switch_root' to boot the main operating system (Whether that be stored on the EMMC Userdata partition or on an external SD Card.

Now some of the issues and my ways around them over the past year:
Use something like petitboot inside of the initrd to kexec another kernel --> This does not work due to the Oppo prebuilt kernel not having kexec support built in.

Booting an operating system that uses SystemD --> This could theoretically be possible but the prebuilt kernel does not have Devtmpfs support built in, and therefore you have to use a tool like busybox's mdev to create your /dev partition.

Build a new kernel from Oppo's open-source github (https://github.com/oppo-source/a91) --> This kernel does not boot, I have been trying for ages and it does not even boot to the point of saying 'Booting linux on physical cpu 0x0' or whatever the first line is. And this is after you patch the damn thing to fix the like 30 errors.

Edit kernel cmdline inside the boot image to change which root partition it boots to --> LK appends to the kernel cmdline without reguard to your feelings.

Port UBoot to the board in place of the boot kernel, and have it handle booting instead of the shitty LK --> I got UBoot booting (although it is in EL2), but I do not have the skills to properly bring up a device tree for this.

With recent advancements I made with running a custom Preloader / LK on this board without Secure boot getting in the way, it might be possible to port UBoot to run in EL3 instead of LK, but after some real quick and dirty testing I did not get anywhere with this.

Anyway more information about this phone's memory map and UBoot porting attempts are availible in a different part of this repository.

So in other words, we are stuck with the prebuilt kernel (@ version 4.4.189). Not bad but it would be handy to have those two features compiled in, or at least a compiling kernel from the source.

Anyway, this does not really matter for general use, only for the initial bring-up development of the initrd and the rootfs (Like swapping udev / SystemD for mdev / OpenRC)

At this time (2024/07/01), the only operating system booting on this is Alpine Linux. Debian wont even do anything, and Gentoo just hangs in init forever. Chrooting into a Debian system from Busybox Initrd does work, but for some unknown reason crashes the whole system into a reboot in ~10 minutes. But both the Busybox Initrd and Alpine Linux run flawlessly for 1+ hours of uptime (I haven't got to testing too much yet).

Okay so now how to assemble this Initrd.

All that there is to it is a Busybox install, and a small init script. I tried to include kexec but it would trip AVB in LK and would make LK panic. Turns out that would have been pointless anyway.

So the first thing to do is to pull the boot.img off of the phone with MTKClient. PLEASE use a boot.img that you have already rooted with Magisk. DM-Verity and AVB are VERY picky on this device and I have not tested any other way.

Anyway, and then use a tool like 'mktool' (https://github.com/GameTheory-/mktool) to extract the boot.img into its kernel and ramdisk/initrd files.
Note: This program will ONLY work on amd64 systems. If you want to use this on arm64 for example, you have to build and swap out the 'mkbootimg' and 'unpackbootimg' executables from https://github.com/osm0sis/mkbootimg

Now it is time to build the initrd.

I am using busybox 1.36.0, and I have provided the sources just in case something breaks in the future.

Now it is time to make your initrd filesystem structure.
mkdir initramfs
mkdir -p initramfs/bin initramfs/sbin initramfs/etc initramfs/proc initramfs/sys initramfs/dev initramfs/usr/bin initramfs/usr/sbin

Now extract the busybox sources, and run 'make menuconfig' to configure it.
The only things we need to change here are to enable 'Settings --> Build static binary (no shared libs)' and change the 'Cross compile prefix' to 'aarch64-linux-gnu-'. This is from the debian package 'gcc-aarch64-linux-gnu'
From there just run 'make -j (cores)'
Now it might seem a little against the norm, but run 'make install' NOT as root, which installs it to the busybox/_install folder.
Now just run 'cp -a busybox/_install/* initramfs/' and now it is mostly finished.
The only thing to do now to finish the file structure is to copy in an init script, of which is the 'init' file in this directory. Make sure it has +x permissions.

Now to compress it into the right format. CD into the initramfs folder, and run: 'find . | cpio -o -H newc | gzip -9 >../initrd.gz' and you are finished. If something went wrong, i mean you are reading a guide on how to build a bare-metal linux system for a phone so I am sure you can figure it out.

Lastly is to just replace the ramdisk file in the mktool extracted folder with the initrd.gz (Rename it to exactly the old ramdisk name), and repack the image. The output boot file to put onto the boot partition on the phone with MTKClient is in the output/ folder.

If you are not interested in building the initrd yourself, and for some reason want to run bare-metal linux on this cursed-ass board, I have provided the initrd in a normal archive, and in it's ready-to-flash drop-in replacement CPIO tar archive as well as 'TODO' and 'TODO' respectively. (CPIO is a pain in the ass to extract)
