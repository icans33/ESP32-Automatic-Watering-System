#pragma once
#include "SystemState.h"

namespace ManualMode
{
  bool start(SystemStatus& status);
  void update(SystemStatus& status);
  void cancel(SystemStatus& status);
}
