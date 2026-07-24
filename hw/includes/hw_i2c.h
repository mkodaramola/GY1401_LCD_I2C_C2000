#ifndef HW_HW_I2C_H_
#define HW_HW_I2C_H_

#include <stdbool.h>
#include <stdint.h>

#include "driverlib.h"
#include "device.h"

namespace HwI2C
{
    static const uint32_t kBase = I2CA_BASE;
    static const uint32_t kBitRateHz = 100000UL;
    static const uint32_t kTimeoutLoops = 50000UL;

    void Init();
    bool Write(uint16_t targetAddress, const uint8_t *data, uint16_t length);
}

#endif
