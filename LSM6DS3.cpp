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

#include "mbed.h"
#include "LSM6DS3.h"

LSM6DS3::LSM6DS3(int address):
    LSM6DS{address} {
}

LSM6DS3::LSM6DS3(PinName sda, PinName scl, int address, uint32_t frequency):
    LSM6DS{sda, scl, address, frequency} {
}

bool LSM6DS3::init(I2C *i2c_obj) {
    if (!LSM6DS::init(i2c_obj)) {
        return false;
    }

    if (!checkWhoAmI(LSM6DS3_WHOAMI)) {
        return false;
    }

    if (!updateGyroScale()) {
        return false;
    }

    return updateAccelScale();
}

bool LSM6DS3::setAccelMode(lsm6ds3_accel_odr_t odr, lsm6ds3_accel_scale_t scale, lsm6ds3_accel_filter_t filter) {
    char data[1];

    // set XL_BW_SCAL_ODR=1 otherwise out new filter will not be used
    if (!readRegister(REG_CTRL4_C, data)) {
        return false;
    }

    data[0] |= 0b10000000;

    if (!writeRegister(REG_CTRL4_C, data)) {
        return false;
    }

    if (!LSM6DS::setAccelMode((char)odr, (char)scale, (char)filter)) {
        return false;
    }

    return updateAccelScale();
}

bool LSM6DS3::updateAccelScale() {
    char data[1];

    if (!readRegister(REG_CTRL1_XL, data)) {
        return false;
    }

    switch ((data[0] >> 2) & 0b11) {
        case AccelScale_2G:
            _accel_scale = 2;
            break;

        case AccelScale_16G:
            _accel_scale = 16;
            break;

        case AccelScale_4G:
            _accel_scale = 4;
            break;

        case AccelScale_8G:
            _accel_scale = 8;
            break;
    }

    return true;
}

bool LSM6DS3::significantMovement(bool enable, char threshold) {
    char data[1];

    if (enable) {
        data[0] = 0b10000000;

        if (!writeRegister(REG_FUNC_CFG_ACCESS, data)) {
            return false;
        }

        data[0] = threshold;

        if (!writeRegister(REG_SM_THS, data)) {
            return false;
        }

        data[0] = 0;

        if (!writeRegister(REG_FUNC_CFG_ACCESS, data)) {
            return false;
        }
    }

    // enable significant motion detection
    if (!readRegister(REG_CTRL10_C, data)) {
        return false;
    }

    data[0] &= ~0b101;
    data[0] |= (char)enable; // SIGN_MOTION_EN
    data[0] |= (char)enable << 2; // FUNC_EN

    if (!writeRegister(REG_CTRL10_C, data)) {
        return false;
    }

    // enable interrupt
    if (!readRegister(REG_INT1_CTRL, data)) {
        return false;
    }

    data[0] &= ~0b01000000;
    data[0] |= (char)enable << 6; // INT1_SIGN_MO

    if (!writeRegister(REG_INT1_CTRL, data)) {
        return false;
    }

    return true;
}

float LSM6DS3::temperatureToC(int16_t raw) {
    return ((float)raw / 16.0) + 25.0;
}

float LSM6DS3::temperatureToF(int16_t raw) {
    return (temperatureToC(raw) * 9 / 5 + 32);
}
