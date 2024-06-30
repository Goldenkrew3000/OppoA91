ramdisk-new-bb
find . | cpio -o -H newc | gzip -9 >../initrd-new.gz
