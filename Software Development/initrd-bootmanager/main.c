#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/fb.h>
#include <pthread.h>
#include <poll.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include "characters.h"

// Local Functions
int startScreen();
void ClearScreen(char *fbmem, int r, int g, int b);
void PlotPixel(char *fbmem, int x, int y, int r, int g, int b);
void DrawRect(char *fbmem, int startx, int starty, int endx, int endy, int padding, int r, int g, int b);
void FillRect(char *fbmem, int startx, int starty, int endx, int endy, int r, int g, int b);
void DrawString(char *fbmem, char* string, int drawx, int drawy, int fr, int fg, int fb, int br, int bg, int bb);
void DrawChar(char *fbmem, char* character, int drawx, int drawy, int fr, int fg, int fb, int br, int bg, int bb);
int monitorTouchscreen();
int checkBounds(int abs_x, int abs_y);
void runDisplayRefresh();
int ReadBattery();
void WriteResultFile(int result);

// Globals
int fb_width;
int fb_height;
int fb_bpp;
int fb_bytes;
int fb_width_virtual;
int fb_height_virtual;
int fb_line_length;

// Button Bounds
int emmc_Xs = 162;
int emmc_Xe = 462;
int emmc_Ys = 707;
int emmc_Ye = 957;
int mmc_Xs = 618;
int mmc_Xe = 918;
int mmc_Ys = 707;
int mmc_Ye = 957;
int pwr_Xs = 162;
int pwr_Xe = 462;
int pwr_Ys = 1257;
int pwr_Ye = 1507;
int rst_Xs = 618;
int rst_Xe = 918;
int rst_Ys = 1257;
int rst_Ye = 1507;
int dbg_Xs = 390;
int dbg_Xe = 690;
int dbg_Ys = 1557;
int dbg_Ye = 1807;

struct input_event {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
};

int main() {
    printf("[INFO] [main] Display Manager started.\n");

    // Start Display Refresher in another thread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, runDisplayRefresh, NULL);

    // Start Display Manager
    int returnedAction = startScreen();

    // Write result to file (Exiting this program in initrd caused system reboot)
    WriteResultFile(returnedAction);

    // Put this program into an infinite loop to avoid system crash
    while (true) {
        usleep(1000000);
    }

    printf("[INFO] [main] Display Manager exited with action %d.\n", returnedAction);
    return 0;
}

int startScreen() {
    // Open framebuffer
    int fbfd = open("/dev/fb0", O_RDWR);
    
    // Check if framebuffer is open
    if (fbfd < 0) {
        printf("[ERR] [startScreen] Could not open /dev/fb0.\n");
    } else {
        printf("[INFO] [startScreen] Opened /dev/fb0.\n");
    }

    assert(fbfd >= 0);

    // Fetch framebuffer info
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
    fb_width = vinfo.xres;
    fb_height = vinfo.yres;
    fb_bpp = vinfo.bits_per_pixel;
    fb_bytes = fb_bpp / 8;
    fb_width_virtual = vinfo.xres_virtual;
    fb_height_virtual = vinfo.yres_virtual;
    fb_line_length = finfo.line_length;

    printf("[INFO] [startScreen] /dev/fb0 specs:\n");
    printf("[INFO] [startScreen] Resolution: %d x %d\n", fb_width, fb_height);
    printf("[INFO] [startScreen] Virtual Resolution: %d x %d\n", fb_width_virtual, fb_height_virtual);
    printf("[INFO] [startScreen] Line Length: %d\n", fb_line_length);
    printf("[INFO] [startScreen] Bits per pixel: %d\n", fb_bpp);

    // Map screen into memory
    int fb_data_size = fb_height_virtual * fb_line_length;
    char *fbdata = mmap(0, fb_data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    // Blank screen (Write 0 to the entire screen)
    printf("[INFO] [startScreen] Initial Screen Clear.\n");
    memset(fbdata, 0, fb_data_size);

    // Clear Screen
    ClearScreen(fbdata, 0xD6, 0xD6, 0xD6); // 0xC2B4FA

    printf("[INFO] [startScreen] Assembling Menu.\n");

    // Print Title
    DrawString(fbdata, "Oppo A91 Boot", 124, 150, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
    DrawString(fbdata, "Built 07/2024", 124, 246, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

    // Print Boot Title
    DrawString(fbdata, "Select Boot:", 156, 550, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

    // Print eMMC Button
    DrawString(fbdata, "eMMC", 184, 800, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6); // 256 wide
    DrawRect(fbdata, 162, 707, 462, 957, 8, 0x00, 0x00, 0x00); // 300 wide, 250 tall, 162 from left

    // Print MMC Button
    DrawString(fbdata, "MMC", 672, 800, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
    DrawRect(fbdata, 618, 707, 918, 957, 8, 0x00, 0x00, 0x00); // 300 wide, 250 tall, 162 from right

    // Print Option Title
    DrawString(fbdata, "Select Option:", 92, 1100, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

    // Print Power Off Button
    DrawString(fbdata, "Pwr", 216, 1350, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
    DrawRect(fbdata, 162, 1257, 462, 1507, 8, 0x00, 0x00, 0x00);

    // Print Reboot Button
    DrawString(fbdata, "Rst", 672, 1350, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
    DrawRect(fbdata, 618, 1257, 918, 1507, 8, 0x00, 0x00, 0x00);

    // Print Debug Button
    DrawString(fbdata, "Dbg", 444, 1650, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
    DrawRect(fbdata, 390, 1557, 690, 1807, 8, 0x00, 0x00, 0x00);

    // Read and Print Battery Percentage
    int batteryPercent = ReadBattery();
    char batteryText[20];
    sprintf(batteryText, "Battery: %d%% \n", batteryPercent); // Yes the space is required.
    DrawString(fbdata, batteryText, 92, 2204, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

    // Print Developer
    DrawString(fbdata, "Goldenkrew3000", 92, 2300, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

    // Start touchscreen monitoring
    int buttonPressed = monitorTouchscreen();

    if (buttonPressed == 1) {
        // Boot eMMC Button
        FillRect(fbdata, emmc_Xs, emmc_Ys, emmc_Xe, emmc_Ye, 0x00, 0x00, 0x00);
        usleep(150000);

        // Clear Screen
        ClearScreen(fbdata, 0xD6, 0xD6, 0xD6);

        // Print Information
        DrawString(fbdata, "Booting eMMC", 156, 1120, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "(/dev/sdc41)", 156, 1200, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

        // Wait 1 Second
        usleep(1000000);

        // Return Action (Let Busybox handle it)
        return 1;
    } else if (buttonPressed == 2) {
        // Boot MMC Button
        FillRect(fbdata, mmc_Xs, mmc_Ys, mmc_Xe, mmc_Ye, 0x00, 0x00, 0x00);
        usleep(150000);

        // Clear Screen
        ClearScreen(fbdata, 0xD6, 0xD6, 0xD6);

        // Print Information
        DrawString(fbdata, "Booting MMC", 188, 1120, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "(/dev/mmcblk0p1)", 28, 1200, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

        // Wait 1 Second
        usleep(1000000);

        // Return Action (Let Busybox handle it)
        return 2;
    } else if (buttonPressed == 3) {
        // Power Button
        FillRect(fbdata, pwr_Xs, pwr_Ys, pwr_Xe, pwr_Ye, 0x00, 0x00, 0x00);
        usleep(150000);

        // Clear Screen
        ClearScreen(fbdata, 0xD6, 0xD6, 0xD6);

        // Print Information
        DrawString(fbdata, "Device will", 188, 1120, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "power off in 3", 92, 1200, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString (fbdata, "seconds...", 220, 1280, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        
        // Wait 3 seconds
        usleep(3000000);

        // Return Action (Let Busybox handle it)
        return 3;
    } else if (buttonPressed == 4) {
        // Reset Button
        FillRect(fbdata, rst_Xs, rst_Ys, rst_Xe, rst_Ye, 0x00, 0x00, 0x00);
        usleep(150000);

        // Clear Screen
        ClearScreen(fbdata, 0xD6, 0xD6, 0xD6);

        // Print Information
        DrawString(fbdata, "Device will", 188, 1120, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "reboot in 3", 188, 1200, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "seconds...", 220, 1280, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

        // Wait 3 seconds
        usleep(3000000);

        // Return Action (Let Busybox handle it)
        return 4;
    } else if (buttonPressed == 5) {
        // Debug Button
        FillRect(fbdata, dbg_Xs, dbg_Ys, dbg_Xe, dbg_Ye, 0x00, 0x00, 0x00);
        usleep(150000);

        // Clear Screen
        ClearScreen(fbdata, 0xD6, 0xD6, 0xD6);

        // Print Information
        DrawString(fbdata, "Debug mode", 220, 1040, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "Console enabled.", 28, 1120, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "/dev/ttyS0", 220, 1200, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);
        DrawString(fbdata, "921600 bps", 220, 1280, 0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6);

        // Wait 1 second
        usleep(1000000);

        // Return Action (Let Busybox handle it)
        return 5;
    }

    // Close framebuffer
    close(fbfd);

    return 0;
}

void PlotPixel(char *fbmem, int x, int y, int r, int g, int b) {
    // Calculate offset
    int offset = (y * fb_line_length) + (x * 4);

    // Write to memory
    fbmem[offset + 0] = r;
    fbmem[offset + 1] = g;
    fbmem[offset + 2] = b;
}

void ClearScreen(char *fbmem, int r, int g, int b) {
    for (int y = 0; y < fb_height_virtual; y++) {
        for (int x = 0; x < fb_width_virtual; x++) {
            if (y <= fb_height && x <= fb_width) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
        }
    }
}

void DrawRect(char *fbmem, int startx, int starty, int endx, int endy, int padding, int r, int g, int b) {
    for (int y = 0; y < fb_height_virtual; y++) {
        for (int x = 0; x < fb_width_virtual; x++) {
            if (y >= starty &&
                y <= starty + padding &&
                x >= startx &&
                x <= endx) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
            if (y >= endy - padding &&
                y <= endy &&
                x >= startx &&
                x <= endx) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
            if (y >= starty &&
                y <= endy &&
                x >= startx &&
                x <= startx + padding) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
            if (y >= starty &&
                y <= endy &&
                x >= endx - padding &&
                x <= endx) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
        }
    }
}

void FillRect(char *fbmem, int startx, int starty, int endx, int endy, int r, int g, int b) {
    for (int y = 0; y < fb_height_virtual; y++) {
        for (int x = 0; x < fb_width_virtual; x++) {
            if (y >= starty &&
                y <= endy &&
                x >= startx &&
                x <= endx) {
                PlotPixel(fbmem, x, y, r, g, b);
            }
        }
    }
}

void DrawString(char *fbmem, char* string, int drawx, int drawy, int fr, int fg, int fb, int br, int bg, int bb) {
    int length = strlen(string);
    int new_drawx = drawx;
    for (int i = 0; i < length; i++) {
        char* new_string = string[i];
        DrawChar(fbmem, &(new_string), new_drawx, drawy, fr, fg, fb, br, bg, bb);
        new_drawx += 64;
    }
}

void DrawChar(char *fbmem, char* character, int drawx, int drawy, int fr, int fg, int fb, int br, int bg, int bb) {
    char current[64];
    if (strncmp(&character[0], "A", 1) == 0) {
        memcpy(current, char_upper_a, 64);
    } else if (strncmp(&character[0], "B", 1) == 0) {
        memcpy(current, char_upper_b, 64);
    } else if (strncmp(&character[0], "C", 1) == 0) {
        memcpy(current, char_upper_c, 64);
    } else if (strncmp(&character[0], "D", 1) == 0) {
        memcpy(current, char_upper_d, 64);
    } else if (strncmp(&character[0], "E", 1) == 0) {
        memcpy(current, char_upper_e, 64);
    } else if (strncmp(&character[0], "F", 1) == 0) {
        memcpy(current, char_upper_f, 64);
    } else if (strncmp(&character[0], "G", 1) == 0) {
        memcpy(current, char_upper_g, 64);
    } else if (strncmp(&character[0], "H", 1) == 0) {
        memcpy(current, char_upper_h, 64);
    } else if (strncmp(&character[0], "I", 1) == 0) {
        memcpy(current, char_upper_i, 64);
    } else if (strncmp(&character[0], "J", 1) == 0) {
        memcpy(current, char_upper_j, 64);
    } else if (strncmp(&character[0], "K", 1) == 0) {
        memcpy(current, char_upper_k, 64);
    } else if (strncmp(&character[0], "L", 1) == 0) {
        memcpy(current, char_upper_l, 64);
    } else if (strncmp(&character[0], "M", 1) == 0) {
        memcpy(current, char_upper_m, 64);
    } else if (strncmp(&character[0], "N", 1) == 0) {
        memcpy(current, char_upper_n, 64);
    } else if (strncmp(&character[0], "O", 1) == 0) {
        memcpy(current, char_upper_o, 64);
    } else if (strncmp(&character[0], "P", 1) == 0) {
        memcpy(current, char_upper_p, 64);
    } else if (strncmp(&character[0], "Q", 1) == 0) {
        memcpy(current, char_upper_q, 64);
    } else if (strncmp(&character[0], "R", 1) == 0) {
        memcpy(current, char_upper_r, 64);
    } else if (strncmp(&character[0], "S", 1) == 0) {
        memcpy(current, char_upper_s, 64);
    } else if (strncmp(&character[0], "T", 1) == 0) {
        memcpy(current, char_upper_t, 64);
    } else if (strncmp(&character[0], "U", 1) == 0) {
        memcpy(current, char_upper_u, 64);
    } else if (strncmp(&character[0], "V", 1) == 0) {
        memcpy(current, char_upper_v, 64);
    } else if (strncmp(&character[0], "W", 1) == 0) {
        memcpy(current, char_upper_w, 64);
    } else if (strncmp(&character[0], "X", 1) == 0) {
        memcpy(current, char_upper_x, 64);
    } else if (strncmp(&character[0], "Y", 1) == 0) {
        memcpy(current, char_upper_y, 64);
    } else if (strncmp(&character[0], "Z", 1) == 0) {
        memcpy(current, char_upper_z, 64);
    } else if (strncmp(&character[0], "0", 1) == 0) {
        memcpy(current, char_number_0, 64);
    } else if (strncmp(&character[0], "1", 1) == 0) {
        memcpy(current, char_number_1, 64);
    } else if (strncmp(&character[0], "2", 1) == 0) {
        memcpy(current, char_number_2, 64);
    } else if (strncmp(&character[0], "3", 1) == 0) {
        memcpy(current, char_number_3, 64);
    } else if (strncmp(&character[0], "4", 1) == 0) {
        memcpy(current, char_number_4, 64);
    } else if (strncmp(&character[0], "5", 1) == 0) {
        memcpy(current, char_number_5, 64);
    } else if (strncmp(&character[0], "6", 1) == 0) {
        memcpy(current, char_number_6, 64);
    } else if (strncmp(&character[0], "7", 1) == 0) {
        memcpy(current, char_number_7, 64);
    } else if (strncmp(&character[0], "8", 1) == 0) {
        memcpy(current, char_number_8, 64);
    } else if (strncmp(&character[0], "9", 1) == 0) {
        memcpy(current, char_number_9, 64);
    } else if (strncmp(&character[0], ":", 1) == 0) {
        memcpy(current, char_colon, 64);
    } else if (strncmp(&character[0], ".", 1) == 0) {
        memcpy(current, char_period, 64);
    } else if (strncmp(&character[0], "!", 1) == 0) {
        memcpy(current, char_exclaim, 64);
    } else if (strncmp(&character[0], "/", 1) == 0) {
        memcpy(current, char_slash, 64);
    } else if (strncmp(&character[0], "%", 1) == 0) {
        memcpy(current, char_percent, 64);
    } else if (strncmp(&character[0], "(", 1) == 0) {
        memcpy(current, char_open_para, 64);
    } else if (strncmp(&character[0], ")", 1) == 0) {
        memcpy(current, char_close_para, 64);
    } else if (strncmp(&character[0], " ", 1) == 0) {
        memcpy(current, char_space, 64);
    } else if (strncmp(&character[0], "a", 1) == 0) {
        memcpy(current, char_lower_a, 64);
    } else if (strncmp(&character[0], "b", 1) == 0) {
        memcpy(current, char_lower_b, 64);
    } else if (strncmp(&character[0], "c", 1) == 0) {
        memcpy(current, char_lower_c, 64);
    } else if (strncmp(&character[0], "d", 1) == 0) {
        memcpy(current, char_lower_d, 64);
    } else if (strncmp(&character[0], "e", 1) == 0) {
        memcpy(current, char_lower_e, 64);
    } else if (strncmp(&character[0], "f", 1) == 0) {
        memcpy(current, char_lower_f, 64);
    } else if (strncmp(&character[0], "g", 1) == 0) {
        memcpy(current, char_lower_g, 64);
    } else if (strncmp(&character[0], "h", 1) == 0) {
        memcpy(current, char_lower_h, 64);
    } else if (strncmp(&character[0], "i", 1) == 0) {
        memcpy(current, char_lower_i, 64);
    } else if (strncmp(&character[0], "j", 1) == 0) {
        memcpy(current, char_lower_j, 64);
    } else if (strncmp(&character[0], "k", 1) == 0) {
        memcpy(current, char_lower_k, 64);
    } else if (strncmp(&character[0], "l", 1) == 0) {
        memcpy(current, char_lower_l, 64);
    } else if (strncmp(&character[0], "m", 1) == 0) {
        memcpy(current, char_lower_m, 64);
    } else if (strncmp(&character[0], "n", 1) == 0) {
        memcpy(current, char_lower_n, 64);
    } else if (strncmp(&character[0], "ol", 1) == 0) {
        memcpy(current, char_lower_o, 64);
    } else if (strncmp(&character[0], "p", 1) == 0) {
        memcpy(current, char_lower_p, 64);
    } else if (strncmp(&character[0], "q", 1) == 0) {
        memcpy(current, char_lower_q, 64);
    } else if (strncmp(&character[0], "r", 1) == 0) {
        memcpy(current, char_lower_r, 64);
    } else if (strncmp(&character[0], "s", 1) == 0) {
        memcpy(current, char_lower_s, 64);
    } else if (strncmp(&character[0], "t", 1) == 0) {
        memcpy(current, char_lower_t, 64);
    } else if (strncmp(&character[0], "u", 1) == 0) {
        memcpy(current, char_lower_u, 64);
    } else if (strncmp(&character[0], "v", 1) == 0) {
        memcpy(current, char_lower_v, 64);
    } else if (strncmp(&character[0], "w", 1) == 0) {
        memcpy(current, char_lower_w, 64);
    } else if (strncmp(&character[0], "x", 1) == 0) {
        memcpy(current, char_lower_x, 64);
    } else if (strncmp(&character[0], "y", 1) == 0) {
        memcpy(current, char_lower_y, 64);
    } else if (strncmp(&character[0], "z", 1) == 0) {
        memcpy(current, char_lower_z, 64);
    }

    for (int i = 1; i < 65; i++) {
        if (current[i-1] == 0xFF) {
            for (int a = 0; a < 8; a++) {
                for (int z = 0; z < 8; z++) {
                    PlotPixel(fbmem, drawx+z, drawy+a, fr, fg, fb);
                }
            }
        } else {
            for (int a = 0; a < 8; a++) {
                for (int z = 0; z < 8; z++) {
                    PlotPixel(fbmem, drawx+z, drawy+a, br, bg, bb);
                }
            }
        }

        if (i % 8 == 0) {
            drawy += 8;
            drawx -= 56;
        } else {
            drawx += 8;
        }          
    } drawy -= 64;
}

int monitorTouchscreen() {
    printf("[INFO] [monitorTouchscreen] Monitoring Touchscreen.\n");
    int ret;
    int abs_x = 0;
    int abs_y = 0;

    struct pollfd fds[1];
    fds[0].fd = open("/dev/input/event2", O_RDONLY|O_NONBLOCK);
    if (fds[0].fd < 0) {
        printf("[ERR] [monitorTouchscreen] Could not open /dev/input/event2.\n");
    } else {
        printf("[INFO] [monitorTouchscreen] Opened /dev/input/event2.\n");
    }

    const int input_size = sizeof(struct input_event);
    struct input_event* input_data;
    input_data = malloc(input_size);
    memset(input_data, 0, input_size);
    fds[0].events = POLLIN;

    while(true) {
        ret = poll(fds, 1, -1);
        if (ret > 0) {
            if (fds[0].revents) {
                size_t r = read(fds[0].fd, input_data, input_size);
                if (r < 0) {
                    printf("[ERR] [monitorTouchscreen] Error occured in reading detected input.\n");
                    break;
                } else {
                    int type = input_data->type;
                    int code = input_data->code;
                    int val = input_data->value;
                    
                    if (type == 3) {
                        if (code == 53) {
                            // Start of new event (53 is always received first), clear abs_x and abs_y
                            abs_x = 0;
                            abs_y = 0;
                            abs_x = val;
                        } else if (code == 54) {
                            abs_y = val;
                        }
                    }

                    memset(input_data, 0, input_size);

                    // Check if new event is complete
                    if (abs_x != 0 && abs_y != 0) {
                        // Check bounds to see if user clicked button
                        int btnReturn = checkBounds(abs_x, abs_y);
                        if (btnReturn > 0) {
                            // User clicked button, return result
                            return btnReturn;
                        }
                    }
                }
            } else {
                printf("[ERR] [monitorTouchscreen] Unknown error occured.\n");
            }
        } else {
            printf("[WARN] [monitorTouchscreen] Timeout occured.\n");
        }
    }

    // Close file descriptor
    close(fds[0].fd);
}

int checkBounds(int abs_x, int abs_y) {
    // Check for emmc button
    if (abs_x >= emmc_Xs && abs_x <= emmc_Xe && abs_y >= emmc_Ys && abs_y <= emmc_Ye) {
        printf("[INFO] [checkBounds] eMMC button pressed.\n");
        return 1;
    }

    // Check for mmc button
    if (abs_x >= mmc_Xs && abs_x <= mmc_Xe && abs_y >= mmc_Ys && abs_y <= mmc_Ye) {
        printf("[INFO] [checkBounds] MMC button pressed.\n");
        return 2;
    }

    // Check for power button
    if (abs_x >= pwr_Xs && abs_x <= pwr_Xe && abs_y >= pwr_Ys && abs_y <= pwr_Ye) {
        printf("[INFO] [checkBounds] Pwr button pressed.\n");
        return 3;
    }

    // Check for reset button
    if (abs_x >= rst_Xs && abs_x <= rst_Xe && abs_y >= rst_Ys && abs_y <= rst_Ye) {
        printf("[INFO] [checkBounds] Rst button pressed.\n");
        return 4;
    }

    // Check for debug button
    if (abs_x >= dbg_Xs && abs_x <= dbg_Xe && abs_y >= dbg_Ys && abs_y <= dbg_Ye) {
        printf("[INFO] [checkBounds] Dbg button pressed.\n");
        return 5;
    }

    return 0;
}

void runDisplayRefresh() {
    printf("[INFO] [displayRefresh] Started Display Refresh.\n");

    struct fb_var_screeninfo var;

    // Open framebuffer file descriptor
    int fbfd = open("/dev/fb0", O_RDWR);

    // Check if framebuffer is open
    if (fbfd < 0) {
        printf("[ERR] [displayRefresh] Could not open /dev/fb0.\n");
    } else {
        printf("[INFO] [displayRefresh] Opened /dev/fb0.\n");
    }
    
    // Fetch framebuffer screen info
    assert(fbfd >= 0);
    assert(ioctl(fbfd, FBIOGET_VSCREENINFO, &var) >= 0);

    while (1) {
        ioctl(fbfd, FBIOPAN_DISPLAY, &var);
        usleep(16666); // Run at ~60fps
    }
}

int ReadBattery() {
    printf("[INFO] [ReadBattery] Reading current battery percentage\n");

    char batteryPercentage[10];
    FILE *batteryfd = fopen("/sys/class/power_supply/battery/capacity", "r");
    if (batteryfd < 0) {
        printf("[ERR] [ReadBattery] Could not open kernel battery monitoring\n");
    } else {
        printf("[INFO] [ReadBattery] Opened kernel battery monitoring\n");
    }

    if (fgets(batteryPercentage, sizeof(batteryPercentage), batteryfd) != NULL) {
        printf("[INFO] [ReadBattery] Read battery percentage from kernel battery monitoring\n");
    } else {
        printf("[ERR] [ReadBattery] Could not read battery percentage from kernel battery monitoring\n");
    }

    // Close battery file descriptor
    fclose(batteryfd);

    return atoi(batteryPercentage);
}

void WriteResultFile(int result) {
    // NOTE: Initrd was crashing when this program exited at all by itself
    // FIX: Write output to /result (its a ramdisk so it will be deleted anyway), and pkill this.
    FILE *fptr;
    fptr = fopen("/result", "w");

    char resultChar[10];
    sprintf(resultChar, "%d", result);

    fprintf(fptr, resultChar);
    fclose(fptr);
    printf("[INFO] [WriteResultFile] Wrote result to file.\n");
}
