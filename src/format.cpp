#include "format.h"

#include <string>

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = seconds / 60;
  int sec = seconds % 60;
  return to_string(hours) + ":" + to_string(minutes) + ":" + to_string(sec);
}