#include "countdown.h"
#include "Arduino.h"

Countdown::Countdown(String label)
{
  this->label = label;
}

void Countdown::setCountdown(int hours, int minutes, int seconds)
{
  this->initialHours = hours;
  this->initialMinutes = minutes;
  this->initialSeconds = seconds;
}

bool Countdown::startCount()
{
  if(!this->started)
  {
    this->started = true;
    this-> endTime = (this->initialSeconds*1000) + (this->initialMinutes * 60 * 1000) + (this->initialHours * 60 * 60 * 1000) + millis();
  }
  else
  {
    long remainingTime = endTime - millis();

    if(remainingTime > 0)
    {
      this -> seconds = remainingTime/1000;
      this -> minutes = seconds/60;
      this -> hours = minutes/60;

      this -> seconds = this -> seconds % 60;
      this -> minutes = this -> minutes % 60;

      this -> done = false;
    }
    else
    {
      this -> done = true;
    }
  }
  return this->done;
}

void Countdown::restart()
{
  this->started = false;
  this->done = false;
}