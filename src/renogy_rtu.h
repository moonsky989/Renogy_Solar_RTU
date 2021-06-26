#include <Arduino.h>

// Read registers
namespace ReadRegisters
{
    const uint16_t RTU_ADDRESS     = 0x001A; // 1-247, lower 8 bits
    const uint16_t BATT_CAPACITY   = 0x0100; // val %
    const uint16_t BATT_CAPACITY_AH = 0xE002; // AH
    const uint16_t BATT_VOLTAGE    = 0x0101; // voltage * 0.1
    const uint16_t CHARGE_CURRENT  = 0x0102; // current * 0.01
    const uint16_t TEMPERATURE     = 0x0103; // 0b-7b controller temp, 8b-15b battery temp, MSB (0b and 9b) is sign, in celcius
    const uint16_t LOAD_VOLTAGE    = 0x0104; // voltage * 0.1
    const uint16_t LOAD_CURRENT    = 0x0105; // current * 0.01
    const uint16_t LOAD_POWER      = 0x0106; // Watts
    const uint16_t PANEL_VOLTAGE   = 0x0107; // voltage * 0.1
    const uint16_t PANEL_CURRENT   = 0x0108; // current * 0.01
    const uint16_t CHARGE_POWER    = 0x0109; // Watts

    const uint16_t BATT_MIN_VOLT    = 0x010B; // daily min. voltage * 0.1
    const uint16_t BATT_MAX_VOLT    = 0x010C; // daily max voltage * 0.1
    const uint16_t CHARGE_MAX_A     = 0x010D; // daily max current * 0.01
    const uint16_t DISCHARGE_MAX_A  = 0x010E; // daily max current * 0.01
    const uint16_t CHARGE_MAX_W     = 0x010F; // daily max Watts
    const uint16_t DISCHARGE_MAX_W  = 0x0110; // daily max Watts
    const uint16_t CHARGE_MAX_AH    = 0x0111; // daily max AH
    const uint16_t DISCHARGE_MAX_AH = 0x0112; // daily max AH

    const uint16_t LOAD_STATUS      = 0x0120; // b7 or b15, 0 - off, 1 - on
}

// Write registers
namespace WriteRegisters
{
    const uint16_t LOAD_CONTROL    = 0x010A; // 1: turn on, 0: turn off
}

namespace Utilities
{
    static std::string RegisterToStr(uint16_t registerVal)
    {
        switch(registerVal)
        {
            case ReadRegisters::RTU_ADDRESS     : return "RTU_ADDRESS";
            case ReadRegisters::BATT_CAPACITY   : return "BATT_CAPACITY";
            case ReadRegisters::BATT_CAPACITY_AH: return "BATT_CAPACITY_AH";
            case ReadRegisters::BATT_VOLTAGE    : return "BATT_VOLTAGE";
            case ReadRegisters::CHARGE_CURRENT  : return "CHARGE_CURRENT";
            case ReadRegisters::TEMPERATURE     : return "TEMPERATURE";
            case ReadRegisters::LOAD_VOLTAGE    : return "LOAD_VOLTAGE";
            case ReadRegisters::LOAD_CURRENT    : return "LOAD_CURRENT";
            case ReadRegisters::LOAD_POWER      : return "LOAD_POWER";
            case ReadRegisters::PANEL_VOLTAGE   : return "PANEL_VOLTAGE";
            case ReadRegisters::PANEL_CURRENT   : return "PANEL_CURRENT";
            case ReadRegisters::CHARGE_POWER    : return "CHARGE_POWER";
            case ReadRegisters::BATT_MIN_VOLT   : return "BATT_MIN_VOLT";
            case ReadRegisters::BATT_MAX_VOLT   : return "BATT_MAX_VOLT";
            case ReadRegisters::CHARGE_MAX_A    : return "CHARGE_MAX_A";
            case ReadRegisters::DISCHARGE_MAX_A : return "DISCHARGE_MAX_A";
            case ReadRegisters::CHARGE_MAX_W    : return "CHARGE_MAX_W";
            case ReadRegisters::DISCHARGE_MAX_W : return "DISCHARGE_MAX_W";
            case ReadRegisters::CHARGE_MAX_AH   : return "CHARGE_MAX_AH";
            case ReadRegisters::DISCHARGE_MAX_AH: return "DISCHARGE_MAX_AH";
            case ReadRegisters::LOAD_STATUS     : return "LOAD_STATUS";
        }
    }
}