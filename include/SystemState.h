#pragma once
#include <Arduino.h>

enum class OperatingMode : uint8_t
{
  OFF,
  MANUAL,
  AUTO,
  INVALID
};

enum class RunState : uint8_t
{
  IDLE,
  VALVE_PREOPEN,
  RUNNING,
  PUMP_STOP_DELAY,
  INTERZONE_DELAY,
  COMPLETE,
  FAULT
};

struct SystemStatus
{
  OperatingMode mode = OperatingMode::OFF;
  RunState runState = RunState::IDLE;

  bool cycleActive = false;
  bool pumpOn = false;

  uint8_t activeZone = 0;       // 0 = none; 1..8 = zone number
  uint8_t selectedZone = 0;     // manual selector result
  uint8_t sequenceIndex = 0;    // auto sequence array index

  unsigned long selectedDurationMs = 0;
  unsigned long remainingMs = 0;

  const char* message = "STARTING";
};
