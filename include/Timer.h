#pragma once
#include <Arduino.h>

class NonBlockingTimer
{
public:
  void start(unsigned long durationMs);
  void stop();
  bool isRunning() const;
  bool expired() const;
  unsigned long elapsed() const;
  unsigned long remaining() const;

private:
  bool running_ = false;
  unsigned long startedAt_ = 0;
  unsigned long duration_ = 0;
};
