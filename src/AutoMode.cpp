#include "AutoMode.h"
#include "Config.h"
#include "IO.h"
#include "ZoneLogic.h"
#include "PumpControl.h"
#include "Timer.h"

namespace
{
  NonBlockingTimer phaseTimer;
  NonBlockingTimer wateringTimer;

  uint8_t sequenceZone(uint8_t index)
  {
    if (index >= AUTO_SEQUENCE_LENGTH) return 0;
    return AUTO_SEQUENCE[index];
  }

  bool openCurrentZone(SystemStatus& status)
  {
    const uint8_t zone = sequenceZone(status.sequenceIndex);

    if (zone < 1 || zone > NUMBER_OF_ZONES)
    {
      status.runState = RunState::FAULT;
      status.message = "SEQ CONFIG ERR";
      return false;
    }

    if (!ZoneLogic::activateZone(zone))
    {
      status.runState = RunState::FAULT;
      status.message = "ZONE ERROR";
      return false;
    }

    status.activeZone = zone;
    status.runState = RunState::VALVE_PREOPEN;
    status.message = "VALVE OPENING";
    phaseTimer.start(VALVE_PREOPEN_MS);

    return true;
  }

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
    status.message = "AUTO COMPLETE";
  }

  void failSafe(SystemStatus& status)
  {
    PumpControl::stop();
    ZoneLogic::deactivateAllZones();

    phaseTimer.stop();
    wateringTimer.stop();

    status.cycleActive = false;
    status.pumpOn = false;
    status.activeZone = 0;
    status.remainingMs = 0;
    status.runState = RunState::FAULT;
  }
}

bool AutoMode::start(SystemStatus& status)
{
  const unsigned long duration = IO::readSelectedDurationMs();

  if (duration == 0)
  {
    status.message = "SELECT TIME";
    return false;
  }

  if (AUTO_SEQUENCE_LENGTH == 0)
  {
    status.runState = RunState::FAULT;
    status.message = "EMPTY SEQUENCE";
    return false;
  }

  PumpControl::stop();
  ZoneLogic::deactivateAllZones();

  status.sequenceIndex = 0;
  status.selectedDurationMs = duration;
  status.remainingMs = duration;
  status.cycleActive = true;
  status.pumpOn = false;

  if (!openCurrentZone(status))
  {
    failSafe(status);
    return false;
  }

  return true;
}

void AutoMode::update(SystemStatus& status)
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
        ZoneLogic::deactivateAllZones();
        status.activeZone = 0;

        if (status.sequenceIndex + 1 >= AUTO_SEQUENCE_LENGTH)
        {
          finish(status);
        }
        else
        {
          status.runState = RunState::INTERZONE_DELAY;
          status.message = "NEXT ZONE";
          phaseTimer.start(INTERZONE_DELAY_MS);
        }
      }
      break;

    case RunState::INTERZONE_DELAY:
      if (phaseTimer.expired())
      {
        status.sequenceIndex++;

        if (!openCurrentZone(status))
        {
          failSafe(status);
        }
      }
      break;

    default:
      break;
  }
}

void AutoMode::cancel(SystemStatus& status)
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
