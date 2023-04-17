#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  explicit Process(std::uint16_t pid);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization();
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;
  void Update();

 private:
  std::uint16_t pid_;
  std::uint64_t previous_total_elapsed_time_;
  std::uint64_t previous_total_process_time_;

  float cpu_utilisation_percentage_{};

  void UpdateCpuUtilization();
};

#endif