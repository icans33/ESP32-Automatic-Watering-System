/*
  Automatic Watering System - ESP32
  Version 1

  Functions:
  - Manual / Off / Auto selector
  - 7-position manual area selector
  - 4-position watering-duration selector
  - Start push button
  - 7 solenoid valve relay outputs
  - Relay 1 pump-contactor output
  - 16x2 I2C LCD
  - One-zone-only software interlock
  - Non-blocking state-machine operation

  Required library:
  - LiquidCrystal_I2C

  IMPORTANT COMMISSIONING ITEMS:
  1. Verify RELAY_ON / RELAY_OFF in Config.h.
  2. Verify the 7-position selector masks in Config.h.
  3. Verify that GPIO36 and GPIO39 have external 10 kOhm pull-ups.
  4. Test with pump contactor isolated before live operation.
*/

#include "Config.h"
#include "SystemState.h"
#include "IO.h"
#include "ZoneLogic.h"
#include "PumpControl.h"
#include "ManualMode.h"
#include "AutoMode.h"
#include "LCDScreen.h"

SystemStatus systemStatus;

namespace
{
  void forceSafeStop(const char* message)
  {
    ManualMode::cancel(systemStatus);
    AutoMode::cancel(systemStatus);

    PumpControl::stop();
    ZoneLogic::deactivateAllZones();

    systemStatus.cycleActive = false;
    systemStatus.pumpOn = false;
    systemStatus.activeZone = 0;
    systemStatus.remainingMs = 0;
    systemStatus.runState = RunState::IDLE;
    systemStatus.message = message;
  }

  void updateIdleSelections()
  {
    if (systemStatus.cycleActive) return;

    systemStatus.selectedDurationMs = IO::readSelectedDurationMs();

    if (systemStatus.mode == OperatingMode::MANUAL)
    {
      systemStatus.selectedZone = IO::readSelectedArea();
    }
    else
    {
      systemStatus.selectedZone = 0;
    }
  }
}

void setup()
{
  // Do not use Serial while GPIO1/GPIO3 are assigned to relays.
  // Serial.begin(115200);

  IO::begin();
  ZoneLogic::begin();
  PumpControl::begin();
  LCDScreen::begin();

  systemStatus.mode = IO::readOperatingMode();
  systemStatus.runState = RunState::IDLE;
  systemStatus.message = "READY";

  delay(500);
}

void loop()
{
  const OperatingMode newMode = IO::readOperatingMode();

  // Any mode change during operation immediately stops the system.
  if (newMode != systemStatus.mode)
  {
    forceSafeStop("MODE CHANGED");
    systemStatus.mode = newMode;
  }

  // OFF and invalid selector states always force safe outputs.
  if (systemStatus.mode == OperatingMode::OFF)
  {
    if (systemStatus.cycleActive ||
        PumpControl::isRunning() ||
        ZoneLogic::activeZone() != 0)
    {
      forceSafeStop("SYSTEM OFF");
    }

    systemStatus.runState = RunState::IDLE;
    systemStatus.message = "SYSTEM OFF";
    LCDScreen::update(systemStatus);
    return;
  }

  if (systemStatus.mode == OperatingMode::INVALID)
  {
    forceSafeStop("CHECK M-O-A");
    systemStatus.runState = RunState::FAULT;
    LCDScreen::update(systemStatus);
    return;
  }

  updateIdleSelections();

  // START is accepted only while idle.
  if (!systemStatus.cycleActive && IO::startButtonPressed())
  {
    if (systemStatus.mode == OperatingMode::MANUAL)
    {
      ManualMode::start(systemStatus);
    }
    else if (systemStatus.mode == OperatingMode::AUTO)
    {
      AutoMode::start(systemStatus);
    }
  }

  if (systemStatus.mode == OperatingMode::MANUAL)
  {
    ManualMode::update(systemStatus);
  }
  else if (systemStatus.mode == OperatingMode::AUTO)
  {
    AutoMode::update(systemStatus);
  }

  systemStatus.pumpOn = PumpControl::isRunning();
  systemStatus.activeZone = ZoneLogic::activeZone();

  LCDScreen::update(systemStatus);
}
