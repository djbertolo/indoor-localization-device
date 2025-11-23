#include "tire/interfaces/RaspberryPiHardware.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <memory>
#include <array>
#include <sstream>
#include <thread>
#include <algorithm>

// ISM330DHCX I2C Address and Registers
#define IMU_ADDRESS 0x6A
#define REG_WHO_AM_I 0x0F
#define REG_CTRL1_XL 0x10 // Accelerometer Control
#define REG_CTRL2_G  0x11 // Gyroscope Control
#define REG_OUTX_L_G 0x22 // Gyroscope Output Start
#define REG_OUTX_L_A 0x28 // Accelerometer Output Start

// GPIO Pins
#define PIN_POWER_SWITCH 4 // Based on schematic
#define PIN_SPEAKER_PWM 12 // Based on schematic

namespace tire {
namespace interfaces {

    RaspberryPiHardware::RaspberryPiHardware() : i2c_fd(-1) {}

    RaspberryPiHardware::~RaspberryPiHardware() {
        // Reset or cleanup if necessary
    }

    bool RaspberryPiHardware::initialize() {
        std::cout << "[RaspberryPiHardware] Initializing..." << std::endl;

        // 1. Initialize WiringPi
        if (wiringPiSetupGpio() == -1) {
            std::cerr << "[RaspberryPiHardware] Error: Failed to init WiringPi." << std::endl;
            return false;
        }

        // 2. Initialize Power Switch
        pinMode(PIN_POWER_SWITCH, INPUT);
        pullUpDnControl(PIN_POWER_SWITCH, PUD_UP); // Assume active low or switch to GND

        // 3. Initialize Keypad Pins
        for (int pin : ROW_PINS) {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, HIGH); // Set rows high
        }
        for (int pin : COL_PINS) {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_UP); // Pull cols up
        }

        // 4. Initialize I2C for IMU
        i2c_fd = wiringPiI2CSetup(IMU_ADDRESS);
        if (i2c_fd == -1) {
            std::cerr << "[RaspberryPiHardware] Error: Failed to init I2C device." << std::endl;
            // Depending on strictness, return false or continue
        } else {
            init_imu_registers();
        }

        // 5. Initialize BLE (Check if bluetooth is up)
        // We assume the system service 'bluetooth' is running.
        int bt_status = std::system("hciconfig hci0 up");
        if (bt_status != 0) {
             std::cerr << "[RaspberryPiHardware] Warning: Could not bring up hci0." << std::endl;
        }

        std::cout << "[RaspberryPiHardware] Initialization Complete." << std::endl;
        return true;
    }

    void RaspberryPiHardware::init_imu_registers() {
        if (i2c_fd == -1) return;

        // Check device ID
        int who_am_i = wiringPiI2CReadReg8(i2c_fd, REG_WHO_AM_I);
        std::cout << "[RaspberryPiHardware] IMU WHO_AM_I: 0x" << std::hex << who_am_i << std::dec << std::endl;

        // Configure Accelerometer: 52Hz, 2g scale
        wiringPiI2CWriteReg8(i2c_fd, REG_CTRL1_XL, 0x30); 
        
        // Configure Gyroscope: 52Hz, 250dps
        wiringPiI2CWriteReg8(i2c_fd, REG_CTRL2_G, 0x30);
    }

    int16_t RaspberryPiHardware::read_i2c_word(int reg_low) {
        // Read low byte
        int l = wiringPiI2CReadReg8(i2c_fd, reg_low);
        // Read high byte
        int h = wiringPiI2CReadReg8(i2c_fd, reg_low + 1);
        return (int16_t)((h << 8) | l);
    }

    IMUData RaspberryPiHardware::read_IMU() {
        IMUData data = {0};
        if (i2c_fd == -1) return data;

        // Read Gyro (X, Y, Z)
        // Scaling factor for 250dps approx 8.75 mdps/LSB (milli-degrees per sec)
        // We want rad/s. 
        // 1 dps = 0.01745 rad/s
        const double GYRO_SCALE = (8.75 / 1000.0) * (3.14159 / 180.0);
        data.gyroscope_x = read_i2c_word(REG_OUTX_L_G) * GYRO_SCALE;
        data.gyroscope_y = read_i2c_word(REG_OUTX_L_G + 2) * GYRO_SCALE;
        data.gyroscope_z = read_i2c_word(REG_OUTX_L_G + 4) * GYRO_SCALE;

        // Read Accel (X, Y, Z)
        // Scaling factor for 2g approx 0.061 mg/LSB
        // We want g or m/s^2. Let's use g.
        const double ACCEL_SCALE = 0.061 / 1000.0; 
        data.acceleration_x = read_i2c_word(REG_OUTX_L_A) * ACCEL_SCALE;
        data.acceleration_y = read_i2c_word(REG_OUTX_L_A + 2) * ACCEL_SCALE;
        data.acceleration_z = read_i2c_word(REG_OUTX_L_A + 4) * ACCEL_SCALE;

        return data;
    }

    std::vector<BLEBeaconData> RaspberryPiHardware::scan_BLE() {
        std::vector<BLEBeaconData> beacons;

        // Execute hcitool scan. 
        // NOTE: This requires sudo permissions or proper capability setting on hcitool.
        // We use --duplicates to get continuous RSSI updates.
        // We limit with 'timeout' to prevent blocking forever.
        const char* cmd = "sudo timeout 1s hcitool lescan --duplicates";
        
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        
        if (!pipe) {
            std::cerr << "[RaspberryPiHardware] Error: popen() failed!" << std::endl;
            return beacons;
        }

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            std::string line = buffer.data();
            // Expected output format: "MAC_ADDRESS NAME"
            // We actually need RSSI, so we might need 'btmgmt' or 'hcidump' for real RSSI.
            // For simplicity in this snippet, we are simulating RSSI random variation 
            // if hcitool doesn't provide it easily without parsing raw dump.
            
            // NOTE: A robust implementation would use 'hcidump --raw' parsing 
            // or the BlueZ C API (HCI sockets).
            
            // Extract MAC
            if (line.length() > 17) {
                std::string mac = line.substr(0, 17);
                
                // Check if we already added this MAC
                bool found = false;
                for(auto& b : beacons) {
                    if(b.id == mac) found = true;
                }

                if(!found) {
                    // Placeholder RSSI because standard 'lescan' doesn't output it textually.
                    // Real implementation requires 'btmgmt find' or parsing hcidump.
                    beacons.push_back({mac, -60}); 
                }
            }
        }
        return beacons;
    }

    KeyPress RaspberryPiHardware::get_key_press() {
        // Matrix Keypad Scan Algorithm
        // 4 Rows, 3 Cols
        
        // Define key map
        KeyPress key_map[4][3] = {
            {KeyPress::KEYCODE_COLUMN_1_UP, KeyPress::KEYCODE_COLUMN_2_UP, KeyPress::KEYCODE_COLUMN_3_UP},     // Row 0
            {KeyPress::KEYCODE_COLUMN_1_DOWN, KeyPress::KEYCODE_COLUMN_2_DOWN, KeyPress::KEYCODE_COLUMN_3_DOWN}, // Row 1
            {KeyPress::KEYCODE_COLUMN_4_UP, KeyPress::KEYCODE_COLUMN_4_DOWN, KeyPress::KEY_CURRENT_SELECTION},   // Row 2
            {KeyPress::KEY_WHERE_AM_I, KeyPress::KEY_START_NAVIGATION, KeyPress::KEY_NONE}                       // Row 3
        };

        for (int r = 0; r < 4; ++r) {
            // Activate Row (LOW)
            digitalWrite(ROW_PINS[r], LOW);

            for (int c = 0; c < 3; ++c) {
                // Check Col (LOW means pressed because of Pull-Up)
                if (digitalRead(COL_PINS[c]) == LOW) {
                    
                    // Simple debounce
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                    if (digitalRead(COL_PINS[c]) == LOW) {
                        
                        // Wait for release? Or just return.
                        // For responsiveness, we return immediately but might block repeat.
                        
                        // Deactivate Row before returning
                        digitalWrite(ROW_PINS[r], HIGH);
                        return key_map[r][c];
                    }
                }
            }
            // Deactivate Row (HIGH)
            digitalWrite(ROW_PINS[r], HIGH);
        }

        return KeyPress::KEY_NONE;
    }

    void RaspberryPiHardware::play_audio(const std::string& audio_cue_name) {
        // Construct system command to play wav file
        // Assumes audio files are in "data/audio/"
        std::stringstream ss;
        ss << "aplay -q data/audio/" << audio_cue_name << ".wav &"; // & for non-blocking
        std::system(ss.str().c_str());
    }

    bool RaspberryPiHardware::is_power_switch_on() {
        return digitalRead(PIN_POWER_SWITCH) == HIGH; // Assuming HIGH is ON
    }

} // namespace interfaces
} // namespace tire