#include "ManualMode.h"
#include "Config.h"
#include "IO.h"
#include "ZoneLogic.h"
#include "PumpControl.h"
#include "Timer.h"

namespace
{
  NonBlockingTimer phaseTimer;
  NonBlockingTimer wateringTimer;

  void finish(SystemStatus& status)
  {
    PumpControl::stop();
    ZoneLogic::deactivateAllZones();

    phaseTimer.stop();
    wateringTimer.stop();

    status.cycleActive = false;
    status.pumpOn = false;
    status.activeZone = 0;
    status.remainingMs = 0;
    status.runState = RunState::COMPLETE;
    status.message = "MANUAL COMPLETE";
  }
}

bool ManualMode::start(SystemStatus& status)
{
  const uint8_t zone = IO::readSelectedArea();
  const unsigned long duration = IO::readSelectedDurationMs();

  if (zone == 0)
  {
    status.message = "SELECT AREA";
    return false;
  }

  if (duration == 0)
  {
    status.message = "SELECT TIME";
    return false;
  }

  PumpControl::stop();
  ZoneLogic::deactivateAllZones();

  if (!ZoneLogic::activateZone(zone))
  {
    status.runState = RunState::FAULT;
    status.message = "ZONE ERROR";
    return false;
  }

  status.selectedZone = zone;
  status.activeZone = zone;
  status.selectedDurationMs = duration;
  status.remainingMs = duration;
  status.cycleActive = true;
  status.pumpOn = false;
  status.runState = RunState::VALVE_PREOPEN;
  status.message = "VALVE OPENING";

  phaseTimer.start(VALVE_PREOPEN_MS);
  return true;
}

void ManualMode::update(SystemStatus& status)
{
  if (!status.cycleActive) return;

  switch (status.runState)
  {
    case RunState::VALVE_PREOPEN:
      if (phaseTimer.expired())
      {
        PumpControl::start();
        wateringTimer.start(status.selectedDurationMs);

        status.pumpOn = true;
        status.runState = RunState::RUNNING;
        status.message = "WATERING";
      }
      break;

    case RunState::RUNNING:
      status.remainingMs = wateringTimer.remaining();

      if (wateringTimer.expired())
      {
        PumpControl::stop();
        status.pumpOn = false;
        status.remainingMs = 0;
        status.runState = RunState::PUMP_STOP_DELAY;
        status.message = "DEPRESSURIZE";

        phaseTimer.start(PRESSURE_RELIEF_MS);
      }
      break;

    case RunState::PUMP_STOP_DELAY:
      if (phaseTimer.expired())
      {
        finish(status);
      }
      break;

    default:
      break;
  }
}

void ManualMode::cancel(SystemStatus& status)
{
  PumpControl::stop();
  ZoneLogic::deactivateAllZones();

  phaseTimer.stop();
  wateringTimer.stop();

  status.cycleActive = false;
  status.pumpOn = false;
  status.activeZone = 0;
  status.remainingMs = 0;
  status.runState = RunState::IDLE;
  status.message = "CANCELLED";
}
