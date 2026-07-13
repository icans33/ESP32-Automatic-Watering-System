#include "Timer.h"

void NonBlockingTimer::start(unsigned long durationMs)
{
  startedAt_ = millis();
  duration_ = durationMs;
  running_ = true;
}

void NonBlockingTimer::stop()
{
  running_ = false;
  startedAt_ = 0;
  duration_ = 0;
}

bool NonBlockingTimer::isRunning() const
{
  return running_;
}

bool NonBlockingTimer::expired() const
{
  return running_ && (millis() - startedAt_ >= duration_);
}

unsigned long NonBlockingTimer::elapsed() const
{
  if (!running_) return 0;
  return millis() - startedAt_;
}

unsigned long NonBlockingTimer::remaining() const
{
  if (!running_) return 0;

  const unsigned long used = millis() - startedAt_;
  if (used >= duration_) return 0;

  return duration_ - used;
}
