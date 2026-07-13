#pragma once
#include <Arduino.h>

// ===================== GENERAL =====================
constexpr uint8_t NUMBER_OF_ZONES = 7;
constexpr uint8_t MANUAL_SELECTABLE_ZONES = 7;

// Most 12 V opto-isolated relay modules are active LOW.
// Change these two values if your relay board is active HIGH.
constexpr uint8_t RELAY_ON  = LOW;
constexpr uint8_t RELAY_OFF = HIGH;

// =====================================================
// RELAY OUTPUT ASSIGNMENT
// =====================================================
//
// Relay 1 = Pump contactor
//
// Relay 2 = Area 1
// Relay 3 = Area 2
// Relay 4 = Area 3
// Relay 5 = Area 4
// Relay 6 = Area 5
// Relay 7 = Area 6
// Relay 8 = Area 7

// Relay 2-8: irrigation solenoid valves
constexpr uint8_t ZONE_RELAY_PINS[NUMBER_OF_ZONES] =
{
  19, // Area 1 - Relay 2
  23, // Area 2 - Relay 3
  2,  // Area 3 - Relay 4
  4,  // Area 4 - Relay 5
  5,  // Area 5 - Relay 6
  32, // Area 6 - Relay 7
  33  // Area 7 - Relay 8
};

// Relay 1 controls the pump contactor.
constexpr uint8_t PUMP_RELAY_PIN = 18;

// ===================== INPUT PINS =====================
constexpr uint8_t START_BUTTON_PIN = 15;

// Schneider Manual-Off-Auto selector
// GPIO36 and GPIO39 are input-only and have no internal pull-up.
// External 10 kOhm pull-ups to 3.3 V are required.
constexpr uint8_t MANUAL_MODE_PIN = 36;
constexpr uint8_t AUTO_MODE_PIN   = 39;

// 7-position voltmeter selector inputs: R, S, T, N
constexpr uint8_t AREA_R_PIN = 12;
constexpr uint8_t AREA_S_PIN = 13;
constexpr uint8_t AREA_T_PIN = 14;
constexpr uint8_t AREA_N_PIN = 27;

// 4-position duration selector
constexpr uint8_t DURATION_1_MIN_PIN  = 16;
constexpr uint8_t DURATION_2_MIN_PIN  = 17;
constexpr uint8_t DURATION_5_MIN_PIN  = 25;
constexpr uint8_t DURATION_10_MIN_PIN = 26;

// ===================== LCD =====================
constexpr uint8_t LCD_ADDRESS = 0x27;
constexpr uint8_t LCD_COLUMNS = 16;
constexpr uint8_t LCD_ROWS = 2;
constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;

// ===================== TIMING =====================
constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;
constexpr unsigned long SELECTOR_DEBOUNCE_MS = 80;

// Valve opens before pump starts.
constexpr unsigned long VALVE_PREOPEN_MS = 1500;

// Pump stops before the valve closes.
constexpr unsigned long PRESSURE_RELIEF_MS = 1500;

// Dead time between closing the old zone and opening the next zone.
constexpr unsigned long INTERZONE_DELAY_MS = 1000;

// LCD refresh interval.
constexpr unsigned long LCD_REFRESH_MS = 250;

// ===================== INPUT POLARITY =====================
// This firmware assumes switches connect the GPIO to GND when active.
constexpr uint8_t INPUT_ACTIVE = LOW;

// ===================== DURATION TABLE =====================
constexpr unsigned long MINUTE_MS = 60000UL;
constexpr unsigned long DURATION_VALUES_MS[4] =
{
  1UL * MINUTE_MS,
  2UL * MINUTE_MS,
  5UL * MINUTE_MS,
  10UL * MINUTE_MS
};

// ===================== AREA SELECTOR MAPPING =====================
// IMPORTANT:
// A 7-position voltmeter selector normally creates combinations of R/S/T/N
// contacts. The exact truth table depends on the switch cam arrangement.
//
// Each bit below represents an ACTIVE input:
// bit 0 = R, bit 1 = S, bit 2 = T, bit 3 = N.
//
// The default table below assumes these common phase-to-phase / phase-neutral
// combinations:
// Position 1 R-S, Position 2 S-T, Position 3 T-R,
// Position 4 R-N, Position 5 S-N, Position 6 T-N,
// Position 7 R-S-T-N.
//
// VERIFY THIS TABLE WITH A CONTINUITY METER DURING COMMISSIONING.
// Change only this table if your switch produces different combinations.
constexpr uint8_t AREA_SELECTOR_MASKS[MANUAL_SELECTABLE_ZONES] =
{
  0b0011, // Area 1: R + S
  0b0110, // Area 2: S + T
  0b0101, // Area 3: T + R
  0b1001, // Area 4: R + N
  0b1010, // Area 5: S + N
  0b1100, // Area 6: T + N
  0b1111  // Area 7: R + S + T + N
};

// ===================== AUTO SEQUENCE =====================
// Zone numbers are human-readable: 1 through 8.
// Reorder or remove entries during commissioning.
constexpr uint8_t AUTO_SEQUENCE[] = {1, 2, 3, 4, 5, 6, 7};
constexpr uint8_t AUTO_SEQUENCE_LENGTH =
  sizeof(AUTO_SEQUENCE) / sizeof(AUTO_SEQUENCE[0]);

// ===================== ZONE NAMES =====================
// Keep names short for the 16-character LCD.
constexpr const char* ZONE_NAMES[NUMBER_OF_ZONES] =
{
  "AREA 1",
  "AREA 2",
  "AREA 3",
  "AREA 4",
  "AREA 5",
  "AREA 6",
  "AREA 7"
};
