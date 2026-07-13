#include "IO.h"
#include "Config.h"

namespace
{
  bool isActive(uint8_t pin)
  {
    return digitalRead(pin) == INPUT_ACTIVE;
  }

  bool rawStartButton()
  {
    return isActive(START_BUTTON_PIN);
  }
}

void IO::begin()
{
  // GPIO36 and GPIO39 require the external pull-ups specified in the panel.
  pinMode(MANUAL_MODE_PIN, INPUT);
  pinMode(AUTO_MODE_PIN, INPUT);

  // Internal pull-ups are used for the remaining dry-contact inputs.
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);

  pinMode(AREA_R_PIN, INPUT_PULLUP);
  pinMode(AREA_S_PIN, INPUT_PULLUP);
  pinMode(AREA_T_PIN, INPUT_PULLUP);
  pinMode(AREA_N_PIN, INPUT_PULLUP);

  pinMode(DURATION_1_MIN_PIN, INPUT_PULLUP);
  pinMode(DURATION_2_MIN_PIN, INPUT_PULLUP);
  pinMode(DURATION_5_MIN_PIN, INPUT_PULLUP);
  pinMode(DURATION_10_MIN_PIN, INPUT_PULLUP);
}

OperatingMode IO::readOperatingMode()
{
  const bool manual = isActive(MANUAL_MODE_PIN);
  const bool automatic = isActive(AUTO_MODE_PIN);

  if (manual && !automatic) return OperatingMode::MANUAL;
  if (!manual && automatic) return OperatingMode::AUTO;
  if (!manual && !automatic) return OperatingMode::OFF;

  return OperatingMode::INVALID;
}

uint8_t IO::readAreaMask()
{
  uint8_t mask = 0;

  if (isActive(AREA_R_PIN)) mask |= 0b0001;
  if (isActive(AREA_S_PIN)) mask |= 0b0010;
  if (isActive(AREA_T_PIN)) mask |= 0b0100;
  if (isActive(AREA_N_PIN)) mask |= 0b1000;

  return mask;
}

uint8_t IO::readSelectedArea()
{
  const uint8_t mask = readAreaMask();

  for (uint8_t i = 0; i < MANUAL_SELECTABLE_ZONES; i++)
  {
    if (mask == AREA_SELECTOR_MASKS[i])
    {
      return i + 1;
    }
  }

  return 0;
}

uint8_t IO::countActiveDurationInputs()
{
  uint8_t count = 0;
  if (isActive(DURATION_1_MIN_PIN)) count++;
  if (isActive(DURATION_2_MIN_PIN)) count++;
  if (isActive(DURATION_5_MIN_PIN)) count++;
  if (isActive(DURATION_10_MIN_PIN)) count++;
  return count;
}

unsigned long IO::readSelectedDurationMs()
{
  // Exactly one position must be active.
  if (countActiveDurationInputs() != 1)
  {
    return 0;
  }

  if (isActive(DURATION_1_MIN_PIN))  return DURATION_VALUES_MS[0];
  if (isActive(DURATION_2_MIN_PIN))  return DURATION_VALUES_MS[1];
  if (isActive(DURATION_5_MIN_PIN))  return DURATION_VALUES_MS[2];
  if (isActive(DURATION_10_MIN_PIN)) return DURATION_VALUES_MS[3];

  return 0;
}

bool IO::startButtonPressed()
{
  static bool stableState = false;
  static bool lastRawState = false;
  static unsigned long lastChangeAt = 0;

  const bool rawState = rawStartButton();

  if (rawState != lastRawState)
  {
    lastRawState = rawState;
    lastChangeAt = millis();
  }

  if (millis() - lastChangeAt >= BUTTON_DEBOUNCE_MS)
  {
    if (rawState != stableState)
    {
      stableState = rawState;

      // Generate one event only on the press edge.
      if (stableState)
      {
        return true;
      }
    }
  }

  return false;
}
