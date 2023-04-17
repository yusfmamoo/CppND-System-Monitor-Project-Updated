#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace {
constexpr std::size_t kBlankCpuStatIndex{2U};
constexpr std::size_t kPidStatusUidIndex{1U};
constexpr std::size_t kPidStatusStartTimeIndex{21U};

constexpr std::size_t kPasswordsUserIndex{0U};
constexpr std::size_t kPasswordsUserIdIndex{2U};
}  // namespace

string GetLine(std::ifstream& filestream, const std::string& key) {
  std::string line, word;
  std::string return_value{"0"};
  while (std::getline(filestream, line)) {
    if (line.find(key) != std::string::npos) {
      std::replace(line.begin(), line.end(), '\t', ' ');
      std::stringstream status_line(line);
      std::vector<string> split_line;
      while (std::getline(status_line, word, ' ')) {
        if (word.find_first_not_of(' ') != std::string::npos) {
          split_line.push_back(word);
        }
      }
      return_value = split_line[1];
      break;
    }
  }
  return return_value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

std::vector<std::string> LinuxParser::PidStat(const std::uint16_t& pid) {
  std::string line, word;
  std::vector<std::string> pid_status;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream single_line(line);
    while (std::getline(single_line, word, ' ')) {
      pid_status.push_back(word);
    }
  }
  return pid_status;
}

float LinuxParser::MemoryUtilization() {
  float memory_utilization{0.0F};
  std::ifstream filestream{kProcDirectory + kMeminfoFilename};
  if (filestream.is_open()) {
    const auto total_memory = std::stof(GetLine(filestream, "MemTotal"));
    const auto available_memory =
        std::stof(GetLine(filestream, "MemAvailable"));

    memory_utilization = available_memory / total_memory;
  }
  return memory_utilization;
}

long LinuxParser::UpTime() {
  std::string line, up_time;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::getline(std::istringstream(line), up_time, ' ');
  }
  return std::stol(up_time);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
  std::string line, word;
  std::vector<std::string> cpu_utilization_stats{};
  std::ifstream filestream{kProcDirectory + kStatFilename};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream single_line(line);
      while (std::getline(single_line, word, ' ')) {
        cpu_utilization_stats.push_back(word);
      }
      if (cpu_utilization_stats.front() == "cpu") {
        cpu_utilization_stats.erase(
            cpu_utilization_stats.begin(),
            cpu_utilization_stats.begin() + kBlankCpuStatIndex);
        break;
      }
    }
  }
  return cpu_utilization_stats;
}

int LinuxParser::TotalProcesses() {
  int return_value{0U};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    return_value = std::stoi(GetLine(filestream, "processes"));
  }
  return return_value;
}

int LinuxParser::RunningProcesses() {
  int return_value{0U};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    return_value = std::stoi(GetLine(filestream, "procs_running"));
  }
  return return_value;
}

string LinuxParser::Command(int pid) {
  std::string command_line{"-"};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, command_line);
  }
  return command_line;
}

string PidStatus(const int& pid, const std::string& status_member) {
  std::string return_value{"0"};
  std::ifstream filestream{LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatusFilename};
  if (filestream.is_open()) {
    return_value = GetLine(filestream, status_member);
  }
  return return_value;
}

string LinuxParser::Ram(int pid) { return PidStatus(pid, "VmSize"); }

string LinuxParser::Uid(int pid) { return PidStatus(pid, "Uid"); }

string LinuxParser::User(int pid) {
  std::string line, word, user{"-"};
  std::ifstream filestream{kPasswordPath};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream single_line(line);
      std::vector<std::string> user_info{};
      while (std::getline(single_line, word, ':')) {
        user_info.push_back(word);
      }
      if (user_info[kPasswordsUserIdIndex] == Uid(pid)) {
        user = user_info[kPasswordsUserIndex];
        break;
      }
    }
  }
  return user;
}

long LinuxParser::UpTime(int pid) {
  if (!PidStat(pid).empty()) {
    return std::stol(PidStat(pid)[kPidStatusStartTimeIndex]);
  }
  return 0;
}