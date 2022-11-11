# LSM6DS
![build](https://github.com/pilotak/LSM6DS/workflows/build/badge.svg)
[![Framework Badge mbed](https://img.shields.io/badge/framework-mbed-008fbe.svg)](https://os.mbed.com/)

Mbed library for LSM6DS3 & LSM6DSO & LSM6DSO32 accel &amp; gyro. Supports only I2C

## Accelerometer examples

<details>
<summary>Basic read</summary>

```cpp
#include "mbed.h"
#include "LSM6DSO.h"

#define I2C_SDA PB_9
#define I2C_SCL PB_8

LSM6DSO accel(I2C_SDA, I2C_SCL);

int main() {
    if (!accel.init()) {
        printf("Could not init\n");
        return 0;
    }

    if (!accel.setupAccel(LSM6DSO::AccelODR_26Hz, LSM6DSO::AccelScale_4G)) {
        printf("Couldn't setup setup slow ODR accel\n");
        return 0;
    }

    while (1) {
        float x, y, z;

        if (!accel.getAccel(&x, &y, &z)) {
            printf("Error reading\n");
            return 0;
        }

        printf("%.6f,%.6f,%.6f\n", x, y, z);

        ThisThread::sleep_for(100ms);
    }
}
```

