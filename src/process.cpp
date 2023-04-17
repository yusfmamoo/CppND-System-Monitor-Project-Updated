#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

namespace {
constexpr std::size_t kUTimeIndex = 13;
constexpr std::size_t kSTimeIndex = 14;
constexpr std::size_t kCUTimeIndex = 15;
constexpr std::size_t kCSTimeIndex = 16;
}  // namespace

void Process::UpdateCpuUtilization() {
  const auto pid_status_information = LinuxParser::PidStat(pid_);
  const std::uint64_t total_process_time =
      std::stol(pid_status_information[kUTimeIndex]) +
      std::stol(pid_status_information[kSTimeIndex]) +
      std::stol(pid_status_information[kCUTimeIndex]) +
      std::stol(pid_status_information[kCSTimeIndex]);
  const std::uint64_t uptime = LinuxParser::UpTime(pid_);
  const std::uint64_t total_elapsed_time =
      LinuxParser::UpTime() - (uptime / sysconf(_SC_CLK_TCK));

  cpu_utilisation_percentage_ =
      (static_cast<float>(total_process_time - previous_total_process_time_) /
       static_cast<float>(sysconf(_SC_CLK_TCK)) /
       static_cast<float>(total_elapsed_time - previous_total_elapsed_time_));

  previous_total_elapsed_time_ = total_elapsed_time;
  previous_total_process_time_ = total_process_time;
}

Process::Process(std::uint16_t pid)
    : pid_(pid),
      previous_total_elapsed_time_(0U),
      previous_total_process_time_(0U),
      cpu_utilisation_percentage_(0.0F) {
  UpdateCpuUtilization();
}

void Process::Update() { UpdateCpuUtilization(); }

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() { return cpu_utilisation_percentage_; }

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const {
  auto ram_in_kB = LinuxParser::Ram(pid_);
  float ram_in_mB{};
  if (ram_in_kB != "0") {
    ram_in_mB = std::stof(ram_in_kB) / 1024;
  }
  return std::to_string(ram_in_mB).substr(0, 6);
}

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const {
  return LinuxParser::UpTime() -
         (LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK));
}

bool Process::operator<(Process const& a) const {
  return a.cpu_utilisation_percentage_ < this->cpu_utilisation_percentage_;
}