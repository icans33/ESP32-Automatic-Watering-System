#pragma once
#include "SystemState.h"

namespace LCDScreen
{
  void begin();
  void update(const SystemStatus& status);
}
