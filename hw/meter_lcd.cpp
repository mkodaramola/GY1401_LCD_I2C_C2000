#include "meter_lcd.h"

#include <string.h>

#include "device.h"
#include "hw_i2c.h"

#ifdef COM0
#undef COM0
#endif
#ifdef COM1
#undef COM1
#endif
#ifdef COM2
#undef COM2
#endif
#ifdef COM3
#undef COM3
#endif

namespace
{
const uint8_t kCommandBit = 0x80U;
const uint8_t kDisctl = 0x20U;
const uint8_t kModset = 0x40U;
const uint8_t kEvrset = 0x60U;
const uint8_t kIcset = 0x68U;
const uint8_t kBlkctl = 0x70U;
const uint8_t kApctl = 0x78U;

const uint8_t kDisctl72Hz = 0x00U;
const uint8_t kDisctlLineInversion = 0x00U;
const uint8_t kDisctlNormalCurrent = 0x02U;
const uint8_t kModsetNormalPower = 0x00U;
const uint8_t kModsetEnable = 0x08U;
const uint8_t kEvrsetHighestContrast = 0x00U;
const uint8_t kIcsetReset = 0x02U;
const uint8_t kBlkctlNoBlink = 0x00U;
const uint8_t kApctlDdramDisplay = 0x00U;
const uint16_t kMaxRamWriteBytes = MeterLcd::kDataSize - 2U;
const uint16_t kCommandDelayUs = 1000U;
const uint8_t kDecimalPointIndex = 7U;
const uint8_t kNibbleSize = 4U;
const uint8_t kDecimalPointPosition = 2U;
const uint8_t kSemicolonPosition = 4U;
const uint8_t kSemicolonCarryPosition = 5U;
const uint8_t kLcdNoOverflowIndex = 9U;
const uint8_t kAsciiZero = 48U;
const uint8_t kErrorCodeMin = 1U;
const uint8_t kErrorCodeMax = 15U;
const uint8_t kErrorCodeDigitCount = 4U;
const uint8_t kEncodedLetterE = 0xF4U;

const uint8_t kEncodedDigit[10] = {
    0xFAU, 0x0AU, 0xBCU, 0x9EU, 0x4EU,
    0xD6U, 0xF6U, 0x8AU, 0xFEU, 0xCEU,
};

uint8_t gDisplayRam[MeterLcd::kRamBytes];

struct SegmentLocation
{
    uint8_t address;
    uint8_t mask;
};

struct DigitLocation
{
    SegmentLocation segment[8];
};

enum SegmentMask
{
    kSegA = 1U << 0U,
    kSegB = 1U << 1U,
    kSegC = 1U << 2U,
    kSegD = 1U << 3U,
    kSegE = 1U << 4U,
    kSegF = 1U << 5U,
    kSegG = 1U << 6U,
    kSegDp = 1U << 7U,
};

uint8_t MakeCommand(uint8_t command)
{
    return static_cast<uint8_t>(kCommandBit | (command & 0x7FU));
}

uint8_t MakeIcset(uint8_t address, bool reset)
{
    uint8_t command = kIcset;

    if ((address & 0x20U) != 0U)
    {
        command |= 0x04U;
    }

    if (reset)
    {
        command |= kIcsetReset;
    }

    if ((address & 0x40U) != 0U)
    {
        command |= 0x01U;
    }

    return command;
}

const SegmentLocation kNoSegment = {0xFFU, 0x00U};

bool IsValidLocation(const SegmentLocation &location)
{
    return (location.address <= MeterLcd::kDdramMaxAddress) && (location.mask != 0U);
}

bool IsMappedSegmentLabel(uint8_t segmentPin)
{
    return (segmentPin <= 86U) && ((segmentPin & 1U) == 0U);
}

SegmentLocation SegmentFromSheet(uint8_t segmentPin, MeterLcd::Com com)
{
    SegmentLocation location;

    if (IsMappedSegmentLabel(segmentPin) == false)
    {
        return kNoSegment;
    }

    location.address =
        static_cast<uint8_t>((segmentPin / 2U) & static_cast<uint8_t>(~1U));
    if ((segmentPin & 0x02U) == 0U)
    {
        location.mask = static_cast<uint8_t>(0x80U >> static_cast<uint8_t>(com));
    }
    else
    {
        location.mask = static_cast<uint8_t>(0x08U >> static_cast<uint8_t>(com));
    }

    return location;
}

DigitLocation DigitFromSheet(uint8_t firstSegmentPin)
{
    DigitLocation digit;
    digit.segment[0] = SegmentFromSheet(firstSegmentPin, MeterLcd::COM0);
    digit.segment[1] = SegmentFromSheet(static_cast<uint8_t>(firstSegmentPin + 2U), MeterLcd::COM0);
    digit.segment[2] = SegmentFromSheet(static_cast<uint8_t>(firstSegmentPin + 2U), MeterLcd::COM2);
    digit.segment[3] = SegmentFromSheet(firstSegmentPin, MeterLcd::COM3);
    digit.segment[4] = SegmentFromSheet(firstSegmentPin, MeterLcd::COM2);
    digit.segment[5] = SegmentFromSheet(firstSegmentPin, MeterLcd::COM1);
    digit.segment[6] = SegmentFromSheet(static_cast<uint8_t>(firstSegmentPin + 2U), MeterLcd::COM1);
    digit.segment[7] = kNoSegment;
    return digit;
}

const DigitLocation kDigitMap[MeterLcd::kDigitCount] = {
    DigitFromSheet(52U),
    DigitFromSheet(56U),
    DigitFromSheet(60U),
    DigitFromSheet(64U),
    DigitFromSheet(70U),
    DigitFromSheet(74U),
    DigitFromSheet(80U),
    DigitFromSheet(84U),
};

const SegmentLocation kGlassSegmentMap[55] = {
    kNoSegment,
    SegmentFromSheet(24U, MeterLcd::COM2),
    SegmentFromSheet(22U, MeterLcd::COM3),
    SegmentFromSheet(24U, MeterLcd::COM3),
    SegmentFromSheet(22U, MeterLcd::COM1),
    SegmentFromSheet(22U, MeterLcd::COM2),
    SegmentFromSheet(20U, MeterLcd::COM1),
    SegmentFromSheet(20U, MeterLcd::COM0),
    SegmentFromSheet(26U, MeterLcd::COM1),
    SegmentFromSheet(22U, MeterLcd::COM0),
    SegmentFromSheet(24U, MeterLcd::COM0),
    SegmentFromSheet(26U, MeterLcd::COM0),
    SegmentFromSheet(24U, MeterLcd::COM1),
    SegmentFromSheet(20U, MeterLcd::COM2),
    SegmentFromSheet(18U, MeterLcd::COM0),
    SegmentFromSheet(16U, MeterLcd::COM1),
    SegmentFromSheet(18U, MeterLcd::COM1),
    SegmentFromSheet(26U, MeterLcd::COM2),
    kNoSegment,
    kNoSegment,
    kNoSegment,
    kNoSegment,
    kNoSegment,
    SegmentFromSheet(26U, MeterLcd::COM3),
    SegmentFromSheet(34U, MeterLcd::COM3),
    SegmentFromSheet(38U, MeterLcd::COM3),
    SegmentFromSheet(42U, MeterLcd::COM3),
    SegmentFromSheet(68U, MeterLcd::COM0),
    SegmentFromSheet(78U, MeterLcd::COM0),
    SegmentFromSheet(72U, MeterLcd::COM3),
    SegmentFromSheet(78U, MeterLcd::COM1),
    SegmentFromSheet(82U, MeterLcd::COM3),
    SegmentFromSheet(16U, MeterLcd::COM0),
    SegmentFromSheet(14U, MeterLcd::COM0),
    SegmentFromSheet(14U, MeterLcd::COM1),
    SegmentFromSheet(14U, MeterLcd::COM2),
    SegmentFromSheet(14U, MeterLcd::COM3),
    SegmentFromSheet(12U, MeterLcd::COM3),
    SegmentFromSheet(12U, MeterLcd::COM2),
    SegmentFromSheet(12U, MeterLcd::COM1),
    SegmentFromSheet(12U, MeterLcd::COM0),
    SegmentFromSheet(10U, MeterLcd::COM0),
    SegmentFromSheet(10U, MeterLcd::COM1),
    SegmentFromSheet(10U, MeterLcd::COM2),
    SegmentFromSheet(10U, MeterLcd::COM3),
    SegmentFromSheet( 8U, MeterLcd::COM3),
    SegmentFromSheet( 8U, MeterLcd::COM1),
    SegmentFromSheet( 6U, MeterLcd::COM1),
    SegmentFromSheet( 4U, MeterLcd::COM1),
    SegmentFromSheet( 0U, MeterLcd::COM1),
    SegmentFromSheet( 8U, MeterLcd::COM2),
    SegmentFromSheet( 6U, MeterLcd::COM2),
    SegmentFromSheet( 2U, MeterLcd::COM1),
    SegmentFromSheet( 0U, MeterLcd::COM2),
    SegmentFromSheet( 0U, MeterLcd::COM3),
};

uint8_t PatternForChar(char value)
{
    switch (value)
    {
        case '0': return kSegA | kSegB | kSegC | kSegD | kSegE | kSegF;
        case '1': return kSegB | kSegC;
        case '2': return kSegA | kSegB | kSegD | kSegE | kSegG;
        case '3': return kSegA | kSegB | kSegC | kSegD | kSegG;
        case '4': return kSegB | kSegC | kSegF | kSegG;
        case '5': return kSegA | kSegC | kSegD | kSegF | kSegG;
        case '6': return kSegA | kSegC | kSegD | kSegE | kSegF | kSegG;
        case '7': return kSegA | kSegB | kSegC;
        case '8': return kSegA | kSegB | kSegC | kSegD | kSegE | kSegF | kSegG;
        case '9': return kSegA | kSegB | kSegC | kSegD | kSegF | kSegG;
        case 'A':
        case 'a': return kSegA | kSegB | kSegC | kSegE | kSegF | kSegG;
        case 'B':
        case 'b': return kSegC | kSegD | kSegE | kSegF | kSegG;
        case 'C':
        case 'c': return kSegA | kSegD | kSegE | kSegF;
        case 'D':
        case 'd': return kSegB | kSegC | kSegD | kSegE | kSegG;
        case 'E':
        case 'e': return kSegA | kSegD | kSegE | kSegF | kSegG;
        case 'F':
        case 'f': return kSegA | kSegE | kSegF | kSegG;
        case 'H':
        case 'h': return kSegB | kSegC | kSegE | kSegF | kSegG;
        case 'L':
        case 'l': return kSegD | kSegE | kSegF;
        case 'O':
        case 'o': return kSegC | kSegD | kSegE | kSegG;
        case 'P':
        case 'p': return kSegA | kSegB | kSegE | kSegF | kSegG;
        case 'U':
        case 'u': return kSegB | kSegC | kSegD | kSegE | kSegF;
        case '-': return kSegG;
        case '_': return kSegD;
        case ' ':
        default: return 0U;
    }
}

bool EncodedMainChar(char value, uint8_t *encoded)
{
    if (encoded == NULL)
    {
        return false;
    }

    if ((value >= '0') && (value <= '9'))
    {
        *encoded = kEncodedDigit[static_cast<uint8_t>(value - kAsciiZero)];
        return true;
    }

    if ((value == 'E') || (value == 'e'))
    {
        *encoded = kEncodedLetterE;
        return true;
    }

    if (value == ' ')
    {
        *encoded = 0U;
        return true;
    }

    return false;
}

void ApplyPattern(uint8_t position, uint8_t pattern)
{
    if (position >= MeterLcd::kDigitCount)
    {
        return;
    }

    const DigitLocation &digit = kDigitMap[position];
    for (uint8_t segment = 0U; segment < 7U; segment++)
    {
        const SegmentLocation &location = digit.segment[segment];
        if (IsValidLocation(location) == false)
        {
            continue;
        }

        if ((pattern & (1U << segment)) != 0U)
        {
            gDisplayRam[location.address] |= location.mask;
        }
        else
        {
            gDisplayRam[location.address] &= static_cast<uint8_t>(~location.mask);
        }
    }
}

bool EncodeMainNumber(const char *text, uint8_t *encoded)
{
    char digits[MeterLcd::kMainMessageDigitCount + 1U];
    uint8_t count = 0U;
    bool hasDecimalPoint = false;

    if ((text == NULL) || (encoded == NULL))
    {
        return false;
    }

    memset(encoded, 0, MeterLcd::kMainMessageBytes);

    for (uint16_t i = 0U; text[i] != '\0'; i++)
    {
        if ((text[i] >= '0') && (text[i] <= '9'))
        {
            if (count < MeterLcd::kMainMessageDigitCount)
            {
                digits[count] = text[i];
                count++;
            }
        }
        else if (text[i] == '.')
        {
            hasDecimalPoint = true;
        }
    }

    if (count == 0U)
    {
        return true;
    }

    uint8_t outputIndex = kLcdNoOverflowIndex;
    uint8_t digitCounter = 0U;
    uint8_t carry = 0U;

    for (uint8_t charIndex = count; charIndex > 0U; charIndex--)
    {
        const uint8_t digit =
            static_cast<uint8_t>(digits[charIndex - 1U] - kAsciiZero);
        const uint8_t lcdValue = kEncodedDigit[digit];

        if (digitCounter < kDecimalPointPosition)
        {
            encoded[outputIndex] = lcdValue;
        }
        else if (digitCounter < kSemicolonPosition)
        {
            if (digitCounter == kDecimalPointPosition)
            {
                encoded[outputIndex] =
                    static_cast<uint8_t>(lcdValue << kNibbleSize);
                if (hasDecimalPoint)
                {
                    encoded[outputIndex] |= 0x04U;
                }
            }
            else
            {
                encoded[outputIndex] =
                    static_cast<uint8_t>((lcdValue << kNibbleSize) | carry);
            }

            carry = static_cast<uint8_t>(lcdValue >> kNibbleSize);
        }
        else if (digitCounter == kSemicolonPosition)
        {
            encoded[outputIndex] = carry;
            charIndex++;
        }
        else
        {
            encoded[outputIndex] = lcdValue;
        }

        digitCounter++;

        if (outputIndex == 0U)
        {
            break;
        }
        outputIndex--;
    }

    if ((digitCounter < kSemicolonCarryPosition) &&
        (digitCounter >= kDecimalPointPosition))
    {
        encoded[outputIndex] = carry;
    }

    return true;
}

bool EncodeMainText(const char *text, uint8_t *encoded)
{
    char chars[MeterLcd::kMainMessageDigitCount + 1U];
    uint8_t count = 0U;

    if ((text == NULL) || (encoded == NULL))
    {
        return false;
    }

    memset(encoded, 0, MeterLcd::kMainMessageBytes);

    for (uint16_t i = 0U; text[i] != '\0'; i++)
    {
        uint8_t lcdValue = 0U;
        if ((EncodedMainChar(text[i], &lcdValue) == true) &&
            (count < MeterLcd::kMainMessageDigitCount))
        {
            chars[count] = text[i];
            count++;
        }
    }

    if (count == 0U)
    {
        return true;
    }

    uint8_t outputIndex = kLcdNoOverflowIndex;
    uint8_t digitCounter = 0U;
    uint8_t carry = 0U;

    for (uint8_t charIndex = count; charIndex > 0U; charIndex--)
    {
        uint8_t lcdValue = 0U;
        if (EncodedMainChar(chars[charIndex - 1U], &lcdValue) == false)
        {
            return false;
        }

        if (digitCounter < kDecimalPointPosition)
        {
            encoded[outputIndex] = lcdValue;
        }
        else if (digitCounter < kSemicolonPosition)
        {
            if (digitCounter == kDecimalPointPosition)
            {
                encoded[outputIndex] =
                    static_cast<uint8_t>(lcdValue << kNibbleSize);
            }
            else
            {
                encoded[outputIndex] =
                    static_cast<uint8_t>((lcdValue << kNibbleSize) | carry);
            }

            carry = static_cast<uint8_t>(lcdValue >> kNibbleSize);
        }
        else if (digitCounter == kSemicolonPosition)
        {
            encoded[outputIndex] = carry;
            charIndex++;
        }
        else
        {
            encoded[outputIndex] = lcdValue;
        }

        digitCounter++;

        if (outputIndex == 0U)
        {
            break;
        }
        outputIndex--;
    }

    if ((digitCounter < kSemicolonCarryPosition) &&
        (digitCounter >= kDecimalPointPosition))
    {
        encoded[outputIndex] = carry;
    }

    return true;
}

bool SendCommand(uint8_t command)
{
    const uint8_t commandByte = MakeCommand(command);
    const bool ok = HwI2C::Write(MeterLcd::kI2cAddress, &commandByte, 1U);
    DEVICE_DELAY_US(kCommandDelayUs);
    return ok;
}

bool WriteRamPage(uint8_t startAddress, const uint8_t *data, uint16_t length)
{
    uint8_t packet[MeterLcd::kRamBytes + 2U];

    if ((data == NULL) || (length == 0U) || (length > kMaxRamWriteBytes))
    {
        return false;
    }

    packet[0] = MeterLcd::kStartAddressCommand;
    packet[1] = startAddress;
    memcpy(&packet[2], data, length);

    return HwI2C::Write(MeterLcd::kI2cAddress, packet, static_cast<uint16_t>(length + 2U));
}
}

void MeterLcd::Init()
{
    memset(gDisplayRam, 0, sizeof(gDisplayRam));

    (void)SendCommand(MakeIcset(0U, true));
    (void)SendCommand(MakeIcset(0U, false));
    (void)SendCommand(kDisctl | kDisctl72Hz |
                      kDisctlLineInversion |
                      kDisctlNormalCurrent);
    (void)SendCommand(kEvrset | kEvrsetHighestContrast);
    (void)SendCommand(kBlkctl | kBlkctlNoBlink);
    (void)SendCommand(kApctl | kApctlDdramDisplay);
    (void)SendCommand(kModset | kModsetNormalPower | kModsetEnable);
    (void)Clear();
}

bool MeterLcd::Clear()
{
    memset(gDisplayRam, 0, sizeof(gDisplayRam));
    return Refresh();
}

bool MeterLcd::Fill()
{
    memset(gDisplayRam, 0xFF, sizeof(gDisplayRam));
    return Refresh();
}

bool MeterLcd::Refresh()
{
    return WriteRam(kDdramStartAddress, gDisplayRam, kDdramByteCount);
}

bool MeterLcd::WriteRam(uint8_t startAddress, const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U) ||
        (startAddress > kDdramMaxAddress) ||
        ((static_cast<uint16_t>(startAddress) + length) >
         (static_cast<uint16_t>(kDdramMaxAddress) + 1U)))
    {
        return false;
    }

    uint8_t address = startAddress;
    const uint8_t *source = data;
    uint16_t remaining = length;

    while (remaining > 0U)
    {
        uint16_t chunkLength = remaining;

        if (chunkLength > kMaxRamWriteBytes)
        {
            chunkLength = kMaxRamWriteBytes;
        }

        if (WriteRamPage(address, source, chunkLength) == false)
        {
            return false;
        }

        address = static_cast<uint8_t>(address + chunkLength);
        source = &source[chunkLength];
        remaining = static_cast<uint16_t>(remaining - chunkLength);
    }

    return true;
}

bool MeterLcd::SetSegment(Com com, uint8_t segmentPin, bool on)
{
    if (com > COM3)
    {
        return false;
    }

    const SegmentLocation location = SegmentFromSheet(segmentPin, com);
    if (IsValidLocation(location) == false)
    {
        return false;
    }

    return SetRamBit(location.address, location.mask, on);
}

bool MeterLcd::SetRawSegment(Com com, uint8_t segmentPin, bool on)
{
    if ((com > COM3) || (IsMappedSegmentLabel(segmentPin) == false))
    {
        return false;
    }

    const uint8_t ddramAddress = static_cast<uint8_t>(segmentPin / 2U);
    const uint8_t comMask = static_cast<uint8_t>(1U << static_cast<uint8_t>(com));

    return SetRamBit(ddramAddress, comMask, on);
}

bool MeterLcd::SetGlassSegment(GlassSegment segment, bool on)
{
    const uint8_t index = static_cast<uint8_t>(segment);

    if (index >= (sizeof(kGlassSegmentMap) / sizeof(kGlassSegmentMap[0])))
    {
        return false;
    }

    const SegmentLocation &location = kGlassSegmentMap[index];
    if (IsValidLocation(location) == false)
    {
        return false;
    }

    return SetRamBit(location.address, location.mask, on);
}

bool MeterLcd::SetRamBit(uint8_t ddramAddress, uint8_t comMask, bool on)
{
    if (ddramAddress > kDdramMaxAddress)
    {
        return false;
    }

    const bool isOn = ((gDisplayRam[ddramAddress] & comMask) != 0U);
    if (isOn == on)
    {
        return true;
    }

    if (on)
    {
        gDisplayRam[ddramAddress] |= comMask;
    }
    else
    {
        gDisplayRam[ddramAddress] &= static_cast<uint8_t>(~comMask);
    }

    return WriteRam(ddramAddress, &gDisplayRam[ddramAddress], 1U);
}

bool MeterLcd::PutChar(uint8_t position, char value)
{
    if (position >= kDigitCount)
    {
        return false;
    }

    const SegmentLocation &dp =
        kDigitMap[position].segment[kDecimalPointIndex];
    const uint8_t dpState = IsValidLocation(dp) ?
        static_cast<uint8_t>(gDisplayRam[dp.address] & dp.mask) : 0U;

    ApplyPattern(position, PatternForChar(value));

    if (dpState != 0U)
    {
        gDisplayRam[dp.address] |= dp.mask;
    }

    return Refresh();
}

bool MeterLcd::SetDecimalPoint(uint8_t position, bool on)
{
    if (position >= kDigitCount)
    {
        return false;
    }

    const SegmentLocation &location =
        kDigitMap[position].segment[kDecimalPointIndex];

    if (IsValidLocation(location) == false)
    {
        return false;
    }

    if (on)
    {
        gDisplayRam[location.address] |= location.mask;
    }
    else
    {
        gDisplayRam[location.address] &= static_cast<uint8_t>(~location.mask);
    }

    return Refresh();
}

bool MeterLcd::Print(const char *text)
{
    uint8_t mainMessage[MeterLcd::kMainMessageBytes];

    if (EncodeMainNumber(text, mainMessage) == false)
    {
        return false;
    }

    memset(gDisplayRam, 0, sizeof(gDisplayRam));
    memcpy(&gDisplayRam[kMainMessageStartAddress],
           mainMessage,
           sizeof(mainMessage));

    return WriteRam(kMainMessageStartAddress,
                    mainMessage,
                    static_cast<uint16_t>(sizeof(mainMessage)));
}

bool MeterLcd::PrintErrorCode(uint8_t errorCode)
{
    uint8_t mainMessage[MeterLcd::kMainMessageBytes];

    if ((errorCode < kErrorCodeMin) || (errorCode > kErrorCodeMax))
    {
        return false;
    }

    char message[kErrorCodeDigitCount + 1U];
    message[0] = 'E';
    message[1] = '0';
    message[2] = static_cast<char>(kAsciiZero + (errorCode / 10U));
    message[3] = static_cast<char>(kAsciiZero + (errorCode % 10U));
    message[4] = '\0';

    if (EncodeMainText(message, mainMessage) == false)
    {
        return false;
    }

    memset(gDisplayRam, 0, sizeof(gDisplayRam));
    memcpy(&gDisplayRam[kMainMessageStartAddress],
           mainMessage,
           sizeof(mainMessage));

    return WriteRam(kMainMessageStartAddress,
                    mainMessage,
                    static_cast<uint16_t>(sizeof(mainMessage)));
}
