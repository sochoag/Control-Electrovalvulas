#include "Arduino.h"

class Countdown
{ 
  private:
    unsigned long endTime = 0;
    bool started = false;
    bool done = false;
    String label;
    unsigned long initialHours, initialMinutes, initialSeconds;
  public:
    unsigned long hours, minutes, seconds;
    Countdown(String label);
    void setCountdown(int hours, int minutes, int seconds);
    bool startCount();
    void restart();
};