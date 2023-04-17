#include "processor.h"

#include <iostream>
#include <numeric>

namespace {
const auto convert_cpu_utilistaion_vector_to_integers =
    [](const std::vector<std::string>& input_string_vector) {
      std::vector<std::uint64_t> integer_vector(input_string_vector.size());
      std::transform(input_string_vector.cbegin(), input_string_vector.cend(),
                     integer_vector.begin(),
                     [](const std::string& input_string) {
                       return std::stoi(input_string);
                     });
      return integer_vector;
    };
}

float Processor::Utilization() {
  static std::uint64_t previous_idle_processes{};
  static std::uint64_t previous_non_idle_processes{};
  static std::uint64_t previous_total_processes{};

  const std::vector<std::uint64_t> cpu_utilisation_stats{
      convert_cpu_utilistaion_vector_to_integers(
          LinuxParser::CpuUtilization())};

  const std::uint64_t current_non_idle_processes =
      cpu_utilisation_stats[LinuxParser::CPUStates::kUser_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kNice_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kSystem_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kIRQ_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kSoftIRQ_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kSteal_];

  const std::uint64_t current_idle_processes =
      cpu_utilisation_stats[LinuxParser::CPUStates::kIdle_] +
      cpu_utilisation_stats[LinuxParser::CPUStates::kIOwait_];

  const std::uint64_t current_total_processes =
      current_non_idle_processes + current_idle_processes;
  previous_total_processes =
      previous_idle_processes + previous_non_idle_processes;

  const float cpu_utilization_percentage =
      (static_cast<float>(current_total_processes - previous_total_processes -
                          (current_idle_processes - previous_idle_processes)) /
       static_cast<float>(current_total_processes - previous_total_processes));

  previous_idle_processes = current_idle_processes;
  previous_non_idle_processes = current_non_idle_processes;

  return cpu_utilization_percentage;
}