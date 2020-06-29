#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iomanip>

#include "linux_parser.h"

using std::stof; // Convert string to float
using std::stol; // Convert string to long
using std::stoi; // Convert string to integer
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
  string os, kernel, version;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float free=0.0, total=0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemFree:"){
        free = stof(value);
      } else if (key == "MemTotal:") {
        total = stof(value);
      }
    }
    
    // Usage is the proportion of used memory over the total memory
    return (total - free) / total;
  }
  
  // If we can't access the file, return 0;
  return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, up, idle;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    // Get the whole line
    std::getline(filestream, line);
    // Make it a stream to take chunks off
    std::istringstream linestream(line);
    // 2 values: uptime in seconds, idle time in seconds
    linestream >> up >> idle;
    return stol(up);
  }
  return 0;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  // Get the CPU details
  vector<string> cpu = LinuxParser::CpuUtilization();
  
  // Store the total
  long total = 0;
  
  // Loop through the values in the vector
  for (int i=0; i<=6; i++){
    total += stol(cpu[i]);
  } 
  
  // Give back the total
  return total;
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    // Get the line
    std::getline(filestream, line);
    // Make it a stream to take chunks off
    std::istringstream linestream(line);
    // Parse tokens
    vector<string> tokens; 
    while (linestream >> value){
      tokens.push_back(value);
    }
    long total;
    
    // Collate uptime (user, kernel, child user, child kernel)
    for (int i = 13; i <= 16; i++){
      total += stol(tokens[i]);
    }
    return total;
  }
  
  // Return default value
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu = LinuxParser::CpuUtilization();
  
  // Grab the fourth (idle) and fifth (IO waiting) values from the cpu details;
  return stol(cpu[3]) + stol(cpu[4]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  // Grabs all the CPU utilization info so other functions can parse it more easily.
  string line, value;
  vector<string> info;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // Snip off the first one
    linestream >> value;
    while (linestream >> value){
      info.push_back(value);
    }
  }
  return info;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, value;
  string line;
  int num = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes"){
        num = stoi(value);
      }
    }
  }
  return num;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, value;
  string line;
  int num = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running"){
        num = stoi(value);
      }
    }
  }
  return num;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line="";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    // Get the whole line
    std::getline(filestream, line);
  }
  return line;
};

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:"){
        std::stringstream result(value);
        
        // Convert to MB (2 d.p)
        result << std::fixed << std::setprecision(2) << stof(value) / 1000;
        
        return result.str();
      }
    }
  }
  return string();
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:"){
        return value;
      }
    }
  }
  return string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string Uid = LinuxParser::Uid(pid);
  string line, key, x, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      
      // Split the line at colons into separate tokens
      std::replace(line.begin(), line.end(), ':', ' ');
      
      // Parse tokens
      std::istringstream linestream(line);
      linestream >> key >> x >> value;
      if (value == Uid) {
        return key;
      }
    }
  }
  
    return string();
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    vector<string> info;
    while(linestream >> value) {
      info.push_back(value);
    }
    
    // total system uptime minus process start time (which needs to be divided by clock ticks)
    return (long)LinuxParser::UpTime() -  stol(info[21]) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
