#include "process_parser.h"
#include <dirent.h>
#include <cmath>
#include <locale>
#include <string>
#include <vector>
#include "util.h"

using std::string;
using std::vector;

// BONUS: Upgrade this to use C++17 std::filesystem
vector<string> ProcessParser::getPidList() {
  vector<string> list;
  DIR* directory = opendir("/proc");
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        list.push_back(filename);
      }
    }
  }
  closedir(directory);
  return list;
}

// TODO: Refactor
string ProcessParser::VmSize(string pid) {
  string line;
  string name{"VmData"};

  std::ifstream stream;
  Util::GetStream(Path::base_path + pid + Path::status_path, stream);
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      return std::to_string(stoi(values[1]) / 1024);
    }
  }
  return string("0");
}

string ProcessParser::Cmd(string pid) { return "/"+pid; }

string ProcessParser::Cpu(string pid) { return "0"+pid; }

string ProcessParser::UpTime(string pid) { return "0"+pid; }

string ProcessParser::User(string pid) { return "0"+pid; }

long int ProcessParser::getSysUpTime() { return 0; }
vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
  return vector<string>{coreNumber};
}
float ProcessParser::getSysRamPercent() { return 0; }
string ProcessParser::getSysKernelVersion() { return string(""); }
int ProcessParser::getNumberOfCores() { return 0; }
int ProcessParser::getTotalThreads() { return 0; }
int ProcessParser::getTotalNumberOfProcesses() { return 0; }
int ProcessParser::getNumberOfRunningProcesses() { return 0; }
string ProcessParser::getOSName() { return string(""); }
string ProcessParser::PrintCpuStats(vector<string> values1,
                                    vector<string> values2) {
  string value;
  if (values1.size() > 0) value += values1[0];
  if (values2.size() > 0) value += values2[0];
  return value;
}
bool ProcessParser::isPidExisting(string pid) { return pid == pid; }