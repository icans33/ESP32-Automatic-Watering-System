#pragma once
#include <Arduino.h>
#include "SystemState.h"

namespace IO
{
  void begin();

  OperatingMode readOperatingMode();
  uint8_t readSelectedArea();
  unsigned long readSelectedDurationMs();

  bool startButtonPressed();

  uint8_t readAreaMask();
  uint8_t countActiveDurationInputs();
}
