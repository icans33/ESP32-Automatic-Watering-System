#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "LCDScreen.h"
#include "Config.h"
#include "ZoneLogic.h"

namespace
{
  LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
  unsigned long lastRefresh = 0;

  void printLine(uint8_t row, const String& text)
  {
    String output = text;

    if (output.length() > LCD_COLUMNS)
    {
      output.remove(LCD_COLUMNS);
    }

    while (output.length() < LCD_COLUMNS)
    {
      output += ' ';
    }

    lcd.setCursor(0, row);
    lcd.print(output);
  }

  String modeText(OperatingMode mode)
  {
    switch (mode)
    {
      case OperatingMode::MANUAL: return "MANUAL";
      case OperatingMode::AUTO: return "AUTO";
      case OperatingMode::OFF: return "OFF";
      default: return "INVALID";
    }
  }

  String durationText(unsigned long durationMs)
  {
    if (durationMs == 0) return "--";

    const unsigned long totalSeconds = durationMs / 1000UL;
    const unsigned long minutes = totalSeconds / 60UL;
    const unsigned long seconds = totalSeconds % 60UL;

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu", minutes, seconds);
    return String(buffer);
  }
}

void LCDScreen::begin()
{
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  printLine(0, "AUTO WATERING");
  printLine(1, "INITIALIZING...");
}

void LCDScreen::update(const SystemStatus& status)
{
  if (millis() - lastRefresh < LCD_REFRESH_MS) return;
  lastRefresh = millis();

  String line1;
  String line2;

  if (status.mode == OperatingMode::INVALID)
  {
    line1 = "SELECTOR ERROR";
    line2 = "CHECK M-O-A";
  }
  else if (status.mode == OperatingMode::OFF)
  {
    line1 = "SYSTEM OFF";
    line2 = "SELECT MAN/AUTO";
  }
  else if (status.runState == RunState::FAULT)
  {
    line1 = "SYSTEM FAULT";
    line2 = status.message;
  }
  else if (status.cycleActive)
  {
    if (status.mode == OperatingMode::AUTO)
    {
      line1 = "AUTO Z";
      line1 += String(status.sequenceIndex + 1);
      line1 += "/";
      line1 += String(AUTO_SEQUENCE_LENGTH);
    }
    else
    {
      line1 = "MAN ";
      line1 += ZoneLogic::zoneName(status.activeZone);
    }

    if (status.runState == RunState::RUNNING)
    {
      line2 = durationText(status.remainingMs);
      line2 += " REMAIN";
    }
    else
    {
      line2 = status.message;
    }
  }
  else
  {
    line1 = modeText(status.mode);
    line1 += " READY";

    if (status.mode == OperatingMode::MANUAL)
    {
      if (status.selectedZone > 0)
      {
        line2 = ZoneLogic::zoneName(status.selectedZone);
      }
      else
      {
        line2 = "SELECT AREA";
      }
    }
    else
    {
      line2 = "PRESS START";
    }
  }

  printLine(0, line1);
  printLine(1, line2);
}
