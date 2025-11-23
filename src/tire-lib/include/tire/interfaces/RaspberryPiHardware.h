#ifndef TIRE_INTERFACES_RASPBERRY_PI_HARDWARE_H
#define TIRE_INTERFACES_RASPBERRY_PI_HARDWARE_H

#include "tire/interfaces/HardwareInterface.h"
#include <atomic>
#include <mutex>

namespace tire {
namespace interfaces {

    /**
     * @class RaspberryPiHardware
     * @brief Concrete implementation of HardwareInterface for the Raspberry Pi 4.
     * * Dependencies:
     * - wiringPi (for GPIO and I2C)
     * - libbluetooth-dev (for BLE scanning)
     * - aplay (system command for audio)
     */
    class RaspberryPiHardware : public HardwareInterface {
    public:
        RaspberryPiHardware();
        virtual ~RaspberryPiHardware() override;

        // --- HardwareInterface Overrides ---
        bool initialize() override;
        IMUData read_IMU() override;
        std::vector<BLEBeaconData> scan_BLE() override;
        KeyPress get_key_press() override;
        void play_audio(const std::string& audio_cue_name) override;
        bool is_power_switch_on() override;

    private:
        // --- Internal Helper Methods ---
        void init_imu_registers();
        int16_t read_i2c_word(int reg_low); // Helper to read 16-bit values

        // --- Member Variables ---
        int i2c_fd; // File descriptor for the I2C IMU

        // Keypad Pin Mappings (Based on schematic)
        // Adjust these if physical wiring differs!
        const std::vector<int> ROW_PINS = {27, 5, 6, 13}; 
        const std::vector<int> COL_PINS = {17, 22, 26};   

        // Debounce tracking
        int last_key_pressed = -1;
        std::chrono::steady_clock::time_point last_press_time;
    };

} // namespace interfaces
} // namespace tire

#endif // TIRE_INTERFACES_RASPBERRY_PI_HARDWARE_H