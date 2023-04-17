#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  const auto pids = LinuxParser::Pids();
  for (const auto& pid : pids) {
    if (std::find_if(processes_.begin(), processes_.end(),
                     [pid](auto& process) { return pid == process.Pid(); }) ==
        processes_.end()) {
      processes_.emplace_back(pid);
    }
  }
  processes_.erase(std::remove_if(processes_.begin(), processes_.end(),
                                  [pids](const auto& process) {
                                    return std::none_of(
                                        pids.begin(), pids.end(),
                                        [process](const auto& value) {
                                          return value == process.Pid();
                                        });
                                  }),
                   processes_.end());

  std::for_each(processes_.begin(), processes_.end(),
                [](auto& process) { process.Update(); });
  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
  ;
}

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
