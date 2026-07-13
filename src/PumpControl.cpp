#include <Arduino.h>
#include "PumpControl.h"
#include "Config.h"

namespace
{
  bool running = false;
}

void PumpControl::begin()
{
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  running = false;
}

void PumpControl::start()
{
  digitalWrite(PUMP_RELAY_PIN, RELAY_ON);
  running = true;
}

void PumpControl::stop()
{
  digitalWrite(PUMP_RELAY_PIN, RELAY_OFF);
  running = false;
}

bool PumpControl::isRunning()
{
  return running;
}
