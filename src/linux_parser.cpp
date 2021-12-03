#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line;
  string memTotal, memFree;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    iss >> memTotal >> memTotal;
    std::getline(stream, line);
    iss.str(line);
    iss.clear();
    iss >> memFree >> memFree;
  }
  if (0 != stoi(memTotal))
    return (stoi(memTotal) - stoi(memFree)) / (float)stoi(memTotal);
  else
    return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  long uptime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    iss >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string token, line;
  long uJiff, nJiff, kJiff, idleJiff, ioJiff, irqJiff, softJiff, stealJiff;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> token;
      if ("cpu" == token) {
        iss >> uJiff >> nJiff >> kJiff >> idleJiff >> ioJiff >> irqJiff >>
            softJiff >> stealJiff;
        return uJiff + nJiff + kJiff + idleJiff + ioJiff + irqJiff + softJiff +
               stealJiff;
      }
    }
  }

  return 0;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, dummy;
  int counter = 14;
  long utime, stime, cutime, cstime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    /* Skip first 13 entry in line */
    while (--counter) {
      iss >> dummy;
    }
    iss >> utime >> stime >> cutime >> cstime;
  }
  /* Are those system clock ticks or jiffies?? */
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string token, line;
  long uJiff, nJiff, kJiff, irqJiff, softJiff, stealJiff, dummy;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> token;
      if ("cpu" == token) {
        iss >> uJiff >> nJiff >> kJiff >> dummy >> dummy >> irqJiff >>
            softJiff >> stealJiff;
        return uJiff + nJiff + kJiff + irqJiff + softJiff + stealJiff;
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string token, line;
  long dummy, idleJiff, ioJiff;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> token;
      if ("cpu" == token) {
        iss >> dummy >> dummy >> dummy >> idleJiff >> ioJiff;
        return idleJiff + ioJiff;
      }
    }
  }
  return 0;
}

// TODO: Read and return CPU utilization
// vector<string> LinuxParser::CpuUtilization() {return {};}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, token;
  int processes;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> token;
      if ("processes" == token) {
        iss >> processes;
        return processes;
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, token;
  int rProcesses;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      iss >> token;
      if ("procs_running" == token) {
        iss >> rProcesses;
        return rProcesses;
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  std::getline(stream, line);
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, token;
  long memSize;
  while (std::getline(stream, line)) {
    std::istringstream iss(line);
    while (iss >> token) {
      if ("VmSize:" == token) {
        iss >> memSize;
        return to_string(memSize / 1000);
      }
    }
  }

  return "";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line, token;
  string uid;
  while (std::getline(stream, line)) {
    std::istringstream iss(line);
    while (iss >> token) {
      if ("Uid:" == token) {
        iss >> uid;
        return uid;
      }
    }
  }

  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  string line, token, user;
  while (std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream iss(line);
    iss >> user;
    /* get uid field */
    iss >> token >> token;
    if (uid == token) return user;
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, dummy;
  int counter = 22;
  long processStartTime;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream iss(line);
    /* Skip first 21 entry in line */
    while (--counter) {
      iss >> dummy;
    }
    iss >> processStartTime;
  }

  return LinuxParser::UpTime() - (processStartTime / sysconf(_SC_CLK_TCK));
}