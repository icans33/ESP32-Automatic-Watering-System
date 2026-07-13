#include "ZoneLogic.h"
#include "Config.h"

namespace
{
  uint8_t currentZone = 0;
}

void ZoneLogic::begin()
{
  // Initialize Relay 2-8 as the seven area outputs.
  //
  // The safe OFF state is written before setting the
  // GPIO as an output to reduce relay activation during
  // initialization.

  for (uint8_t i = 0; i < NUMBER_OF_ZONES; i++)
  {
    digitalWrite(
      ZONE_RELAY_PINS[i],
      RELAY_OFF
    );

    pinMode(
      ZONE_RELAY_PINS[i],
      OUTPUT
    );

    digitalWrite(
      ZONE_RELAY_PINS[i],
      RELAY_OFF
    );
  }

  currentZone = 0;
}

bool ZoneLogic::activateZone(uint8_t zoneNumber)
{
  if (zoneNumber < 1 || zoneNumber > NUMBER_OF_ZONES)
  {
    deactivateAllZones();
    return false;
  }

  // Software interlock: every zone is turned off before one is energized.
  deactivateAllZones();

  digitalWrite(ZONE_RELAY_PINS[zoneNumber - 1], RELAY_ON);
  currentZone = zoneNumber;

  return true;
}

void ZoneLogic::deactivateAllZones()
{
  for (uint8_t i = 0; i < NUMBER_OF_ZONES; i++)
  {
    digitalWrite(ZONE_RELAY_PINS[i], RELAY_OFF);
  }

  currentZone = 0;
}

uint8_t ZoneLogic::activeZone()
{
  return currentZone;
}

const char* ZoneLogic::zoneName(uint8_t zoneNumber)
{
  if (zoneNumber < 1 || zoneNumber > NUMBER_OF_ZONES)
  {
    return "NO ZONE";
  }

  return ZONE_NAMES[zoneNumber - 1];
}
