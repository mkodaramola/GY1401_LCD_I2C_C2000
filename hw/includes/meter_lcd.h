#ifndef HW_METER_LCD_H_
#define HW_METER_LCD_H_
#include "driverlib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

namespace MeterLcd
{
    static const uint16_t kI2cAddress = 0x3EU;
    static const uint16_t kRamBytes = 48U;
    static const uint16_t kDdramByteCount = kRamBytes;
    static const uint8_t kDdramStartAddress = 0x00U;
    static const uint8_t kDdramMaxAddress = 0x2FU;
    static const uint16_t kDataSize = 25U;
    static const uint8_t kStartAddressCommand = 0xE8U;
    static const uint8_t kMainMessageStartAddress = 0x18U;
    static const uint8_t kMainMessageEndAddress = 0x22U;
    static const uint16_t kMainMessageBytes = 11U;
    static const uint16_t kDigitCount = 8U;
    static const uint16_t kMainMessageDigitCount = 10U;

    enum Com
    {
        COM0 = 0U,
        COM1 = 1U,
        COM2 = 2U,
        COM3 = 3U,
    };

    enum GlassSegment
    {
        K1 = 1U,  K2,  K3,  K4,  K5,  K6,  K7,  K8,  K9,
        K10, K11, K12, K13, K14, K15, K16, K17, K18,
        K19, K20, K21, K22, K23, K24, K25, K26, K27,
        K28, K29, K30, K31, K32, K33, K34, K35, K36,
        K37, K38, K39, K40, K41, K42, K43, K44, K45,
        K46, K47, K48, K49, K50, K51, K52, K53, K54,
    };

    void Init();
    bool Clear();
    bool Fill();
    bool Refresh();
    bool WriteRam(uint8_t startAddress, const uint8_t *data, uint16_t length);
    bool SetSegment(Com com, uint8_t segmentPin, bool on);
    bool SetGlassSegment(GlassSegment segment, bool on);
    bool SetRamBit(uint8_t ddramAddress, uint8_t comMask, bool on);
    bool PutChar(uint8_t position, char value);
    bool SetDecimalPoint(uint8_t position, bool on);
    bool Print(const char *text);
}

#define ON true
#define OFF false

#define COM0 MeterLcd::COM0
#define COM1 MeterLcd::COM1
#define COM2 MeterLcd::COM2
#define COM3 MeterLcd::COM3

#define SEG0 0U
#define SEG2 2U
#define SEG4 4U
#define SEG6 6U
#define SEG8 8U
#define SEG10 10U
#define SEG12 12U
#define SEG14 14U
#define SEG16 16U
#define SEG18 18U
#define SEG20 20U
#define SEG22 22U
#define SEG24 24U
#define SEG26 26U
#define SEG28 28U
#define SEG30 30U
#define SEG32 32U
#define SEG34 34U
#define SEG36 36U
#define SEG38 38U
#define SEG40 40U
#define SEG42 42U
#define SEG44 44U
#define SEG46 46U
#define SEG48 48U
#define SEG50 50U
#define SEG52 52U
#define SEG54 54U
#define SEG56 56U
#define SEG58 58U
#define SEG60 60U
#define SEG62 62U
#define SEG64 64U
#define SEG66 66U
#define SEG68 68U
#define SEG70 70U
#define SEG72 72U
#define SEG74 74U
#define SEG76 76U
#define SEG78 78U
#define SEG80 80U
#define SEG82 82U
#define SEG84 84U
#define SEG86 86U

inline bool LCD_SetSegment(MeterLcd::Com com, uint8_t segmentPin, bool on)
{
    return MeterLcd::SetSegment(com, segmentPin, on);
}

#endif
