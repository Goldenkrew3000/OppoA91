#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include "termcolor.hpp"

/*
Thermal Zones (type):
mtktsbattery, mt6358tsbuck1, mt6358tsbuck2, mt6358tsbuck3, mtktscharger,
mtktscharger2, battery, mtktscpu, mtktspmic

Battery Uevent:
POWER_SUPPLY_STATUS --> String (Charging / Not charging)
POWER_SUPPLY_CAPACITY --> Int (Battery Percentage)
POWER_SUPPLY_VOLTAGE_NOW --> Int (Battery Voltage)
POWER_SUPPLY_CURRENT_NOW --> Int (Battery Current, REVERSED. Positive meaning losing, Negative meaning gaining)

USB Uevent:
POWER_SUPPLY_ONLINE --> Int (0 / 1) TO TEST
POWER_SUPPLY_OTG_SWITCH --> Int (0 / 1)
POWER_SUPPLY_OTG_ONLINE --> Int (0 / 1)
*/

// Locations in /sys
std::string SYS_USB_UEVENT = "/sys/class/power_supply/usb/uevent";
std::string SYS_USB_UEVENT_ONLINE = "";
std::string SYS_USB_UEVENT_OTG_SWITCH = "";
std::string SYS_USB_UEVENT_OTG_ONLINE = "";

std::string SYS_BATTERY_UEVENT = "/sys/class/power_supply/battery/uevent";
std::string SYS_BATTERY_UEVENT_STATUS = "";
std::string SYS_BATTERY_UEVENT_CAPACITY = "";
std::string SYS_BATTERY_UEVENT_VOLTAGE_NOW = "";
std::string SYS_BATTERY_UEVENT_CURRENT_NOW = "";

std::string SYS_THERMAL_ROOT = "/sys/class/thermal/thermal_zone"; // /sys/class/thermal/thermal_zone0/type
std::string SYS_THERMAL_TYPE = "/type";
std::string SYS_THERMAL_TEMP = "/temp";
int SYS_THERMAL_COUNT = 26; // 0 - 25
int SYS_THERMAL_INDEX_MTKTSBATTERY = 0;
int SYS_THERMAL_INDEX_MT6358TSBUCK1 = 0;
int SYS_THERMAL_INDEX_MT6358TSBUCK2 = 0;
int SYS_THERMAL_INDEX_MT6358TSBUCK3 = 0;
int SYS_THERMAL_INDEX_MTKTSCHARGER = 0;
int SYS_THERMAL_INDEX_MTKTSCHARGER2 = 0;
int SYS_THERMAL_INDEX_BATTERY = 0;
int SYS_THERMAL_INDEX_MTKTSCPU = 0;
int SYS_THERMAL_INDEX_MTKTSPMIC = 0;

std::string SYS_CPU_FREQ_ROOT = "/sys/devices/system/cpu/cpu"; // /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq
std::string SYS_CPU_FREQ_CURRENT = "/cpufreq/scaling_cur_freq";

// Debug Printing
bool debug = false;

// Functions
void processUsbUevent(std::ifstream& filePtr);
void processBatteryUevent(std::ifstream& filePtr);
void searchThermalZones();
std::string processTemperature(std::ifstream& filePtr);
std::string getCpuFreq(std::ifstream& filePtr);

int main() {
    std::cout << "Mediatek Stats\n\n";

    // Open USB uevent
    std::ifstream fptr_usb_uevent;
    fptr_usb_uevent.open(SYS_USB_UEVENT);

    // Open Battery uevent
    std::ifstream fptr_battery_uevent;
    fptr_battery_uevent.open(SYS_BATTERY_UEVENT);

    // Search Thermal Zones
    searchThermalZones();
    
    while (true) {
        // Process USB and Battery uevent
        processUsbUevent(fptr_usb_uevent);
        processBatteryUevent(fptr_battery_uevent);

        // Get TSBattery Temperature
        std::ifstream fptr_temperature_tsbattery;
        std::string SYS_THERMAL_MTKTSBATTERY = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MTKTSBATTERY) + SYS_THERMAL_TEMP;
        fptr_temperature_tsbattery.open(SYS_THERMAL_MTKTSBATTERY);
        std::string SYS_THERMAL_RES_MTKTSBATTERY = processTemperature(fptr_temperature_tsbattery);

        // Get Mt6358TsBuck1 Temperature
        std::ifstream fptr_temperature_mt6358tsbuck1;
        std::string SYS_THERMAL_MT6358TSBUCK1 = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MT6358TSBUCK1) + SYS_THERMAL_TEMP;
        fptr_temperature_mt6358tsbuck1.open(SYS_THERMAL_MT6358TSBUCK1);
        std::string SYS_THERMAL_RES_MT6358TSBUCK1 = processTemperature(fptr_temperature_mt6358tsbuck1);

        // Get Mt6358TsBuck2 Temperature
        std::ifstream fptr_temperature_mt6358tsbuck2;
        std::string SYS_THERMAL_MT6358TSBUCK2 = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MT6358TSBUCK2) + SYS_THERMAL_TEMP;
        fptr_temperature_mt6358tsbuck2.open(SYS_THERMAL_MT6358TSBUCK2);
        std::string SYS_THERMAL_RES_MT6358TSBUCK2 = processTemperature(fptr_temperature_mt6358tsbuck2);

        // Get Mt6358TsBuck3 Temperature
        std::ifstream fptr_temperature_mt6358tsbuck3;
        std::string SYS_THERMAL_MT6358TSBUCK3 = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MT6358TSBUCK3) + SYS_THERMAL_TEMP;
        fptr_temperature_mt6358tsbuck3.open(SYS_THERMAL_MT6358TSBUCK3);
        std::string SYS_THERMAL_RES_MT6358TSBUCK3 = processTemperature(fptr_temperature_mt6358tsbuck3);

        // Get TsCharger Temperature
        std::ifstream fptr_temperature_tscharger;
        std::string SYS_THERMAL_TSCHARGER = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MTKTSCHARGER) + SYS_THERMAL_TEMP;
        fptr_temperature_tscharger.open(SYS_THERMAL_TSCHARGER);
        std::string SYS_THERMAL_RES_TSCHARGER = processTemperature(fptr_temperature_tscharger);

        // Get TsCharger2 Temperature
        std::ifstream fptr_temperature_tscharger2;
        std::string SYS_THERMAL_TSCHARGER2 = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MTKTSCHARGER2) + SYS_THERMAL_TEMP;
        fptr_temperature_tscharger2.open(SYS_THERMAL_TSCHARGER2);
        std::string SYS_THERMAL_RES_TSCHARGER2 = processTemperature(fptr_temperature_tscharger2);

        // Get Battery Temperature
        std::ifstream fptr_temperature_battery;
        std::string SYS_THERMAL_BATTERY = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_BATTERY) + SYS_THERMAL_TEMP;
        fptr_temperature_battery.open(SYS_THERMAL_BATTERY);
        std::string SYS_THERMAL_RES_BATTERY = processTemperature(fptr_temperature_battery);

        // Get TsCpu Temperature
        std::ifstream fptr_temperature_mtktscpu;
        std::string SYS_THERMAL_MTKTSCPU = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MTKTSCPU) + SYS_THERMAL_TEMP;
        fptr_temperature_mtktscpu.open(SYS_THERMAL_MTKTSCPU);
        std::string SYS_THERMAL_RES_MTKTSCPU = processTemperature(fptr_temperature_mtktscpu);

        // Get TsPmic Temperature
        std::ifstream fptr_temperature_mtktspmic;
        std::string SYS_THERMAL_MTKTSPMIC = "/sys/class/thermal/thermal_zone" + std::to_string(SYS_THERMAL_INDEX_MTKTSPMIC) + SYS_THERMAL_TEMP;
        fptr_temperature_mtktspmic.open(SYS_THERMAL_MTKTSPMIC);
        std::string SYS_THERMAL_RES_MTKTSPMIC = processTemperature(fptr_temperature_mtktspmic);

        // Get Cpu 0-8 Frequencies
        std::ifstream fptr_cpu0_freq;
        std::ifstream fptr_cpu1_freq;
        std::ifstream fptr_cpu2_freq;
        std::ifstream fptr_cpu3_freq;
        std::ifstream fptr_cpu4_freq;
        std::ifstream fptr_cpu5_freq;
        std::ifstream fptr_cpu6_freq;
        std::ifstream fptr_cpu7_freq;
        std::string SYS_CPU0_FREQ = SYS_CPU_FREQ_ROOT + "0" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU1_FREQ = SYS_CPU_FREQ_ROOT + "1" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU2_FREQ = SYS_CPU_FREQ_ROOT + "2" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU3_FREQ = SYS_CPU_FREQ_ROOT + "3" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU4_FREQ = SYS_CPU_FREQ_ROOT + "4" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU5_FREQ = SYS_CPU_FREQ_ROOT + "5" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU6_FREQ = SYS_CPU_FREQ_ROOT + "6" + SYS_CPU_FREQ_CURRENT;
        std::string SYS_CPU7_FREQ = SYS_CPU_FREQ_ROOT + "7" + SYS_CPU_FREQ_CURRENT;
        fptr_cpu0_freq.open(SYS_CPU0_FREQ);
        fptr_cpu1_freq.open(SYS_CPU1_FREQ);
        fptr_cpu2_freq.open(SYS_CPU2_FREQ);
        fptr_cpu3_freq.open(SYS_CPU3_FREQ);
        fptr_cpu4_freq.open(SYS_CPU4_FREQ);
        fptr_cpu5_freq.open(SYS_CPU5_FREQ);
        fptr_cpu6_freq.open(SYS_CPU6_FREQ);
        fptr_cpu7_freq.open(SYS_CPU7_FREQ);
        std::string SYS_CPU0_RES_FREQ = getCpuFreq(fptr_cpu0_freq);
        std::string SYS_CPU1_RES_FREQ = getCpuFreq(fptr_cpu1_freq);
        std::string SYS_CPU2_RES_FREQ = getCpuFreq(fptr_cpu2_freq);
        std::string SYS_CPU3_RES_FREQ = getCpuFreq(fptr_cpu3_freq);
        std::string SYS_CPU4_RES_FREQ = getCpuFreq(fptr_cpu4_freq);
        std::string SYS_CPU5_RES_FREQ = getCpuFreq(fptr_cpu5_freq);
        std::string SYS_CPU6_RES_FREQ = getCpuFreq(fptr_cpu6_freq);
        std::string SYS_CPU7_RES_FREQ = getCpuFreq(fptr_cpu7_freq);

        // Log info
        std::cout << termcolor::bright_yellow << "USB Info\n" << termcolor::reset;
        std::cout << "Is USB Power Connected: ";
        if (SYS_USB_UEVENT_ONLINE == "1") {
            std::cout << termcolor::bright_green << "yes\n" << termcolor::reset;
        } else {
            std::cout << termcolor::bright_red << "no\n" << termcolor::reset;
        }
        std::cout << "Is USB OTG Switch Enabled: ";
        if (SYS_USB_UEVENT_OTG_SWITCH == "1") {
            std::cout << termcolor::bright_green << "yes\n" << termcolor::reset;
        } else {
            std::cout << termcolor::bright_red << "no\n" << termcolor::reset;
        }
        std::cout << "Is USB OTG Switch Online: ";
        if (SYS_USB_UEVENT_OTG_ONLINE == "1") {
            std::cout << termcolor::bright_green << "yes\n" << termcolor::reset;
        } else {
            std::cout << termcolor::bright_red << "no\n" << termcolor::reset;
        }
        std::cout << "\n";

        std::cout << termcolor::bright_yellow << "Battery Info\n" << termcolor::reset;
        std::cout << "Is battery charging: " << SYS_BATTERY_UEVENT_STATUS << "\n";
        std::cout << "Battery remaining: " << SYS_BATTERY_UEVENT_CAPACITY << "\n";
        std::cout << "Battery voltage: " << SYS_BATTERY_UEVENT_VOLTAGE_NOW << "\n";
        std::cout << "Battery current: " << SYS_BATTERY_UEVENT_CURRENT_NOW << "\n";
        std::cout << "\n";

        std::cout << termcolor::bright_yellow << "Temeratures\n" << termcolor::reset;
        std::cout << "TS Battery: " << SYS_THERMAL_RES_MTKTSBATTERY << "\n";
        std::cout << "Battery: " << SYS_THERMAL_RES_BATTERY << "\n";
        std::cout << "TS CPU: " << SYS_THERMAL_RES_MTKTSCPU << "\n";
        std::cout << "TS PMIC: " << SYS_THERMAL_RES_MTKTSPMIC << "\n";
        std::cout << "MT6358 TS Buck 1: " << SYS_THERMAL_RES_MT6358TSBUCK1 << "\n";
        std::cout << "MT6358 TS Buck 2: " << SYS_THERMAL_RES_MT6358TSBUCK2 << "\n";
        std::cout << "MT6358 TS Buck 3: " << SYS_THERMAL_RES_MT6358TSBUCK3 << "\n";
        std::cout << "TS Charger: " << SYS_THERMAL_RES_TSCHARGER << "\n";
        std::cout << "TS Charger 2: " << SYS_THERMAL_RES_TSCHARGER2 << "\n";
        std::cout << "\n";

        std::cout << termcolor::bright_yellow << "CPU (" << termcolor::magenta << "A53" << termcolor::bright_yellow << ") Frequencies\n" << termcolor::reset;
        std::cout << "CPU 0: " << SYS_CPU0_RES_FREQ << "\n";
        std::cout << "CPU 1: " << SYS_CPU1_RES_FREQ << "\n";
        std::cout << "CPU 2: " << SYS_CPU2_RES_FREQ << "\n";
        std::cout << "CPU 3: " << SYS_CPU3_RES_FREQ << "\n";
        std::cout << "\n";

        std::cout << termcolor::bright_yellow << "CPU (" << termcolor::magenta << "A73" << termcolor::bright_yellow << ") Frequencies\n" << termcolor::reset;
        std::cout << "CPU 4: " << SYS_CPU4_RES_FREQ << "\n";
        std::cout << "CPU 5: " << SYS_CPU5_RES_FREQ << "\n";
        std::cout << "CPU 6: " << SYS_CPU6_RES_FREQ << "\n";
        std::cout << "CPU 7: " << SYS_CPU7_RES_FREQ << "\n";
        std::cout << "\n";

        // Wait 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}

void processUsbUevent(std::ifstream& filePtr) {
    std::string line;
    std::string full;

    filePtr.clear();
    filePtr.seekg(0);
    while (std::getline(filePtr, line)) {
        if (line.find("POWER_SUPPLY_ONLINE=") != std::string::npos) {
            SYS_USB_UEVENT_ONLINE = line.substr(20);
        } else if (line.find("POWER_SUPPLY_OTG_SWITCH=") != std::string::npos) {
            SYS_USB_UEVENT_OTG_SWITCH = line.substr(24);
        } else if (line.find("POWER_SUPPLY_OTG_ONLINE=") != std::string::npos) {
            SYS_USB_UEVENT_OTG_ONLINE = line.substr(24);
        }
    }

    if (debug) {
        std::cout << "POWER_SUPPLY_ONLINE --> " << SYS_USB_UEVENT_ONLINE << "\n";
        std::cout << "POWER_SUPPLY_OTG_SWITCH --> " << SYS_USB_UEVENT_OTG_SWITCH << "\n";
        std::cout << "POWER_SUPPLY_OTG_ONLINE --> " << SYS_USB_UEVENT_OTG_ONLINE << "\n";
    }
}

void processBatteryUevent(std::ifstream& filePtr) {
    std::string line;
    std::string full;
    
    filePtr.clear();
    filePtr.seekg(0);
    while (std::getline(filePtr, line)) {
        if (line.find("POWER_SUPPLY_STATUS=") != std::string::npos) {
            SYS_BATTERY_UEVENT_STATUS = line.substr(20);
        } else if (line.find("POWER_SUPPLY_CAPACITY=") != std::string::npos) {
            SYS_BATTERY_UEVENT_CAPACITY = line.substr(22);
            SYS_BATTERY_UEVENT_CAPACITY += "%";
        } else if (line.find("POWER_SUPPLY_VOLTAGE_NOW=") != std::string::npos) {
            SYS_BATTERY_UEVENT_VOLTAGE_NOW = line.substr(25);
            SYS_BATTERY_UEVENT_VOLTAGE_NOW += " mV";
        } else if (line.find("POWER_SUPPLY_CURRENT_NOW=") != std::string::npos) {
            SYS_BATTERY_UEVENT_CURRENT_NOW = line.substr(25);
            SYS_BATTERY_UEVENT_CURRENT_NOW += " mA";
        }
    }
    
    if (debug) {
        std::cout << "POWER_SUPPLY_STATUS --> " << SYS_BATTERY_UEVENT_STATUS << "\n";
        std::cout << "POWER_SUPPLY_CAPACITY --> " << SYS_BATTERY_UEVENT_CAPACITY << "\n";
        std::cout << "POWER_SUPPLY_VOLTAGE_NOW --> " << SYS_BATTERY_UEVENT_VOLTAGE_NOW << "\n";
        std::cout << "POWER_SUPPLY_CURRENT_NOW --> " << SYS_BATTERY_UEVENT_CURRENT_NOW << "\n";
    }
}

void searchThermalZones() {
    for (int i = 0; i < SYS_THERMAL_COUNT; i++) {
        // Generate /sys path
        std::ifstream fptr_thermal_search;
        std::string filename = SYS_THERMAL_ROOT + std::to_string(i) + SYS_THERMAL_TYPE;
        
        fptr_thermal_search.open(filename);

        std::string line;
        fptr_thermal_search.clear();
        fptr_thermal_search.seekg(0);
        fptr_thermal_search >> line;
        
        if (line == "mtktsbattery") {
            SYS_THERMAL_INDEX_MTKTSBATTERY = i;
        } else if (line == "mt6358tsbuck1") {
            SYS_THERMAL_INDEX_MT6358TSBUCK1 = i;
        } else if (line == "mt6358tsbuck2") {
            SYS_THERMAL_INDEX_MT6358TSBUCK2 = i;
        } else if (line == "mt6358tsbuck3") {
            SYS_THERMAL_INDEX_MT6358TSBUCK3 = i;
        } else if (line == "mtktscharger") {
            SYS_THERMAL_INDEX_MTKTSCHARGER = i;
        } else if (line == "mtktscharger2") {
            SYS_THERMAL_INDEX_MTKTSCHARGER2 = i;
        } else if (line == "battery") {
            SYS_THERMAL_INDEX_BATTERY = i;
        } else if (line == "mtktscpu") {
            SYS_THERMAL_INDEX_MTKTSCPU = i;
        } else if (line == "mtktspmic") {
            SYS_THERMAL_INDEX_MTKTSPMIC = i;
        }

        fptr_thermal_search.close();
    }
}

std::string processTemperature(std::ifstream& filePtr) {
    std::string line;
    filePtr.clear();
    filePtr.seekg(0);
    filePtr >> line;
    float line_flt = std::stof(line) / 1000;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << line_flt;
    line = stream.str();
    line += " c";
    return line;
}

std::string getCpuFreq(std::ifstream& filePtr) {
    std::string line;
    filePtr.clear();
    filePtr.seekg(0);
    filePtr >> line;
    line = std::to_string(atoi(line.c_str()) / 1000);
    line += " MHz";
    return line;
}
