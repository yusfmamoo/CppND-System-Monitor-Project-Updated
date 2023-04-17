#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  const std::uint32_t hours = seconds / 3600;
  const std::uint32_t minutes = (seconds / 60) % 60;
  const std::uint32_t remaining_seconds = seconds % 60;

  std::stringstream time_string;
  time_string << std::setw(2) << std::setfill('0') << hours << ":"
              << std::setw(2) << std::setfill('0') << minutes << ":"
              << std::setw(2) << std::setfill('0') << remaining_seconds;
  return time_string.str();
}