#pragma once
#include <Arduino.h>

namespace ZoneLogic
{
  void begin();

  bool activateZone(uint8_t zoneNumber);
  void deactivateAllZones();

  uint8_t activeZone();
  const char* zoneName(uint8_t zoneNumber);
}
