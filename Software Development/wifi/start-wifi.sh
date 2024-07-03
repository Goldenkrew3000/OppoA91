echo Starting Mediatek Wifi Subsystem

echo Loading wmt_drv.ko into kernel...
insmod modules/wmt_drv.ko

echo Reloading /dev for /dev/wmtdetect
mdev -s

echo Running mtinit to power on the IC
./openmttools/mtinit

echo Reloading /dev
mdev -s

echo Loading wmt_chrdev.wiki.ko into kernel
insmod modules/wmt_chrdev_wifi.ko

echo Loading wlan_drv_gen3.ko into kernel
insmod modules/wlan_drv_gen3.ko

echo Reloading /dev for /dev/wmtWifi
mdev -s

echo Running mtdaemon to prepare loading firmware into the modem
./openmttools/mtdaemon &

echo Sleeping 2 seconds to let mtdaemon settle
sleep 2

echo Actually enable wifi
echo 1 > /dev/wmtWifi

echo Sleeping 5 seconds to settle
sleep 5

echo Run wpasupplicant to connect to wifi
wpa_supplicant -iwlan0 -Dnl80211 -cwifi_conf.conf -B

echo Sleeping 2 seconds to let wpa_supplicant settle
sleep 2

echo Obtaining DHCP address
dhclient wlan0 -v
