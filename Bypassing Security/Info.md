Okay so the issue is:

(Gemini) LK AVB Check fails.
Fails: '[avb] ret = 2'
Passes: '[avb] ret = 0'

It attempts to check the 'vbmeta' partition, but this phone does not have a 'vbmeta' partition.

How did I end up in this situation: Wrote a modified preloader to check if it was checked by secureboot, fuck it was.
Now I cannot boot the phone normally (Even when writing the original preloader back), it still kicks itself into MTK EDL.

Note: You can know if your MTK device is in MTK EDL mode by monitoring uart (For this device, BROM is 115200 bps, and Linux / LK is 921600) and look for this line:
'[DL] 00009C40 444CFFFC 010701'
You can still access your device in this mode, but you have to have previously dumped the preloader using MTKClient to do so.

Although this was a little bit of a mistake, it allows one MASSIVE upside. You can now modify the Preloader and LK partitions.
Unfortunately, LK AVB Checking still occurs but this can be patched out now since you can modify LK.

Note: The original LK, LK2, Boot, and Preloader are held inside of the 'Unmodified' folder, along with SHA256 checksums.

Anyway, I am kind of thankful for this error because I needed to figure this out anyway.

As for booting the phone, the preloader is now needed in this state.
I am using mtkclient (https://github.com/bkerler/mtkclient) commit 53e667a. This is included in this repository.

And the command is: python3 mtk.py plstage --preloader=<preloader bin file>
Note: This does not have to be the original preloader, it can be a modified one.
