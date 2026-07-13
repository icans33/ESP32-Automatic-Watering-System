#pragma once
#include "SystemState.h"

namespace AutoMode
{
  bool start(SystemStatus& status);
  void update(SystemStatus& status);
  void cancel(SystemStatus& status);
}
