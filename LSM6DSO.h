/*
MIT License

Copyright (c) 2021 Pavel Slama

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef LSM6DSO_H
#define LSM6DSO_H

#include "LSM6DS.h"

#define LSM6DSO_WHOAMI 0x6C

#define LSM6DSO_FN_SRC_STEP_COUNT_DELTA_IA 0b10000000 // pedometer step recognition on delta
#define LSM6DSO_FN_SRC_SIGN_MOTION_IA      0b1000000 // significant motion event
#define LSM6DSO_FN_SRC_TILT_IA             0b100000 // tilt event detection
#define LSM6DSO_FN_SRC_STEP_DETECTED       0b10000 // step detector even
#define LSM6DSO_FN_SRC_STEP_OVERFLOW       0b1000 // step counter overflow 
#define LSM6DSO_FN_SRC_SI_END_OP           0b10 // hard/soft-iron calculation
#define LSM6DSO_FN_SRC_SENSORHUB_END_OP    0b1 // sensor hub communication

#define LSM6DSO_MD1_CFG_INT1_INACT_STATE 0b10000000 // inactivity mode
#define LSM6DSO_MD1_CFG_INT1_SINGLE_TAP  0b1000000 // single-tap recognition
#define LSM6DSO_MD1_CFG_INT1_WU          0b100000 //  wakeup event
#define LSM6DSO_MD1_CFG_INT1_FF          0b10000 // free-fall event
#define LSM6DSO_MD1_CFG_INT1_DOUBLE_TAP  0b1000 // double-tap event
#define LSM6DSO_MD1_CFG_INT1_6D          0b100 // 6D event
#define LSM6DSO_MD1_CFG_TNT1_TILT        0b10 // tilt event
#define LSM6DSO_MD1_CFG_INT1_TIMER       0b1 // end counter event

#define LSM6DSO_MD2_CFG_INT2_INACT_STATE 0b10000000 // inactivity mode
#define LSM6DSO_MD2_CFG_INT2_SINGLE_TAP  0b1000000 // single-tap recognition
#define LSM6DSO_MD2_CFG_INT2_WU          0b100000 //  wakeup event
#define LSM6DSO_MD2_CFG_INT2_FF          0b10000 // free-fall event
#define LSM6DSO_MD2_CFG_INT2_DOUBLE_TAP  0b1000 // double-tap event
#define LSM6DSO_MD2_CFG_INT2_6D          0b100 // 6D event
#define LSM6DSO_MD2_CFG_TNT1_TILT        0b10 // tilt event
#define LSM6DSO_MD2_CFG_INT2_IRON        0b1 // soft-iron/hard-iron algorithm end event


class LSM6DSO: public LSM6DS {
  public:
    typedef enum {
        AccelODR_PowerDown = 0,
        AccelODR_12_5Hz, // low power
        AccelODR_26Hz,   // low power
        AccelODR_52Hz,   // low power
        AccelODR_104Hz,  // normal mode
        AccelODR_208Hz,  // normal mode
        AccelODR_416Hz,  // high performance
        AccelODR_833Hz,  // high performance
        AccelODR_1666Hz, // high performance
        AccelODR_3333Hz, // high performance
        AccelODR_6666Hz, // high performance
    } LSM6DSO_accel_odr_t;

    typedef enum {
        AccelScale_2G = 0,
        AccelScale_16G,
        AccelScale_4G,
        AccelScale_8G
    } LSM6DSO_accel_scale_t;

    // Anti-aliasing filter
    typedef enum {
        AccelAAFilter_400Hz = 0,
        AccelAAFilter_200Hz,
        AccelAAFilter_100Hz,
        AccelAAFilter_50Hz
    } LSM6DSO_accel_aa_filter_t;

    // Slope, High-pass & Low-pass2 filter
    typedef enum {
        AccelSlopeFilter = 0,
        AccelHPF_1, // ODR/100
        AccelHPF_2, // ODR/9
        AccelHPF_3, // ODR/400
        AccelLPF2_0, // ODR/50
        AccelLPF2_1, // ODR/100
        AccelLPF2_2, // ODR/9
        AccelLPF2_3, // ODR/400
        AccelFilter_Off,
    } LSM6DSO_accel_lhpf_t;

    // High-pass filter cutoff frequency
    typedef enum {
        GyroHPF_0 = 0, // 0.0081 Hz
        GyroHPF_1, // 0.0324 Hz
        GyroHPF_2, // 2.07 Hz
        GyroHPF_3, // 16.32 Hz
        GyroHPF_Off,
    } LSM6DSO_gyro_hpf_t;

    LSM6DSO(int address = LSM6DS_DEFAULT_ADDRESS);
    LSM6DSO(PinName sda, PinName scl, int address = LSM6DS_DEFAULT_ADDRESS, uint32_t frequency = 400000);

    /**
    * @brief Initialise the chip
    *
    * @param i2c_obj pass I2C object if you didn't specify pins in constructor
    * @return true if successful, otherwise false
    */
    bool init(I2C *i2c_obj = nullptr);

    /**
     * @brief Setup the accelerometer
     *
     * @param odr output data rate and power mode selection
     * @param scale full-scale selection
     * @param filter nti-aliasing filter bandwidth selection
     * @return true if successful, otherwise false
     */
    bool setupAccel(LSM6DSO_accel_odr_t odr, LSM6DSO_accel_scale_t scale = AccelScale_2G,
                    LSM6DSO_accel_aa_filter_t filter = AccelAAFilter_400Hz);

    /**
     * @brief Set Slope, high-pass or low-pass2 filter for accelerometer
     *
     * @param filter filter type
     * @param lp_6d enable low-pass filter for 6D (only if filter is off or LPF2 is set)
     * @return true if successful, otherwise false
     */
    bool setAccelFilter(LSM6DSO_accel_lhpf_t filter, bool lp_6d = false);

    /**
     * @brief Set high-pass filter for gyroscope
     *
     * @param filter filter type
     * @return true if successful, otherwise false
     */
    bool setGyroFilter(LSM6DSO_gyro_hpf_t filter);

    /**
     * @brief Enable/disable significant motion detection.
     * Don't forget to enable interrupt too!
     *
     * @param enable
     * @param threshold 0-255
     * @return true if successful, otherwise false
     */
    bool significantMotion(bool enable, char threshold = 6);

    /**
     * @brief Get function interrupt source register
     *
     * @param reason place to put the reading (1 byte)
     * @return true if successful, otherwise false
     */
    bool getFnIntReason(char *reason);

    /**
     * @brief Configure wakeup event
     * Don't forget to enable FN interrupt too!
     *
     * @param threshold range 0-63
     * @param duration range 0-3
     * @param sleep_duration range 0-15
     * @return true if successful, otherwise false
     */
    bool setWakeup(char threshold, char duration, char sleep_duration);

    /**
     * @brief Set the Latched Interrupt mode
     *
     * @param enable
     * @return true if successful, otherwise fals
     */
    bool setIntLatchMode(bool enable);


    /**
     * @brief Enable/disable inactivity detection
     *
     * @param enable
     * @return true if successful, otherwise false
     */
    bool enableInactivity(bool enable);

    /**
     * @brief Set FIFO mode
     *
     * @param gyro_decimation
     * @param accel_decimation
     * @return true if successful, otherwise false
     */
    bool fifoMode(char gyro_decimation, char accel_decimation);

    /**
     * @brief Convert raw temperature reading to °C
     *
     * @param raw reading from getTemperature()
     * @return temperature in °C
     */
    float temperatureToC(int16_t raw);

    /**
     * @brief Convert raw temperature reading to °F
     *
     * @param raw reading from getTemperature()
     * @return temperature in °F
     */
    float temperatureToF(int16_t raw);

  private:
    typedef enum {
        REG_FIFO_CTRL1 = 0x06,
        REG_FIFO_CTRL2 = 0x07,
        REG_FIFO_CTRL3 = 0x08,
        REG_FIFO_CTRL4 = 0x09,
        REG_FIFO_CTRL5 = 0x0A,
        REG_FUNC_SRC = 0x53,
    } LSM6DSO_reg_t;

    /**
     * @brief Update lib accelerometer scale
     *
     * @return true if successful, otherwise false
     */
    bool updateAccelScale();
};

#endif
