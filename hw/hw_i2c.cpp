#include "hw_i2c.h"

namespace
{
const uint32_t kSdaPin = 32U;
const uint32_t kSclPin = 33U;
const uint32_t kSdaPinConfig = GPIO_32_I2CA_SDA;
const uint32_t kSclPinConfig = GPIO_33_I2CA_SCL;
const uint16_t kClearableStatus = I2C_STS_ARB_LOST |
                                  I2C_STS_NO_ACK |
                                  I2C_STS_REG_ACCESS_RDY |
                                  I2C_STS_RX_DATA_RDY |
                                  I2C_STS_STOP_CONDITION |
                                  I2C_STS_BYTE_SENT |
                                  I2C_STS_NACK_SENT |
                                  I2C_STS_TARGET_DIR;

bool WaitForBusFree()
{
    uint32_t timeout = HwI2C::kTimeoutLoops;

    while ((I2C_isBusBusy(HwI2C::kBase) == true) && (timeout > 0UL))
    {
        timeout--;
    }

    return timeout > 0UL;
}

bool WaitForTxFifoEmpty()
{
    uint32_t timeout = HwI2C::kTimeoutLoops;

    while ((I2C_getTxFIFOStatus(HwI2C::kBase) != I2C_FIFO_TXEMPTY) &&
           (timeout > 0UL))
    {
        if ((I2C_getStatus(HwI2C::kBase) &
             (I2C_STS_NO_ACK | I2C_STS_ARB_LOST)) != 0U)
        {
            I2C_sendStopCondition(HwI2C::kBase);
            I2C_clearStatus(HwI2C::kBase, kClearableStatus);
            return false;
        }

        timeout--;
    }

    return timeout > 0UL;
}

bool WaitForStop()
{
    uint32_t timeout = HwI2C::kTimeoutLoops;

    while ((I2C_getStopConditionStatus(HwI2C::kBase) == true) &&
           (timeout > 0UL))
    {
        timeout--;
    }

    return timeout > 0UL;
}
}

void HwI2C::Init()
{
    GPIO_setPinConfig(kSdaPinConfig);
    GPIO_setPinConfig(kSclPinConfig);
    GPIO_setPadConfig(kSdaPin, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(kSclPin, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(kSdaPin, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(kSclPin, GPIO_QUAL_ASYNC);

    I2C_disableModule(kBase);
    I2C_initController(kBase, DEVICE_SYSCLK_FREQ, kBitRateHz, I2C_DUTYCYCLE_50);
    I2C_setAddressMode(kBase, I2C_ADDR_MODE_7BITS);
    I2C_setEmulationMode(kBase, I2C_EMULATION_FREE_RUN);
    I2C_enableFIFO(kBase);
    I2C_setFIFOInterruptLevel(kBase, I2C_FIFO_TXEMPTY, I2C_FIFO_RXFULL);
    I2C_clearStatus(kBase, kClearableStatus);
    I2C_enableModule(kBase);
}

bool HwI2C::Write(uint16_t targetAddress, const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) || (targetAddress > 0x7FU))
    {
        return false;
    }

    if (WaitForBusFree() == false)
    {
        return false;
    }

    I2C_disableFIFO(kBase);
    I2C_enableFIFO(kBase);
    I2C_clearStatus(kBase, kClearableStatus);
    I2C_setTargetAddress(kBase, targetAddress);
    I2C_setConfig(kBase, I2C_CONTROLLER_SEND_MODE);
    I2C_setDataCount(kBase, length);

    uint16_t offset = 0U;
    uint16_t chunkLength = (length > 16U) ? 16U : length;

    for (uint16_t i = 0U; i < chunkLength; i++)
    {
        I2C_putData(kBase, data[offset++]);
    }

    I2C_sendStartCondition(kBase);

    while (offset < length)
    {
        if (WaitForTxFifoEmpty() == false)
        {
            return false;
        }

        chunkLength = ((length - offset) > 16U) ? 16U : (length - offset);
        for (uint16_t i = 0U; i < chunkLength; i++)
        {
            I2C_putData(kBase, data[offset++]);
        }
    }

    if (WaitForTxFifoEmpty() == false)
    {
        return false;
    }

    I2C_sendStopCondition(kBase);

    if (WaitForStop() == false)
    {
        return false;
    }

    I2C_clearStatus(kBase, kClearableStatus);
    return true;
}
