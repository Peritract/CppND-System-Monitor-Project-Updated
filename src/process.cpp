#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;
using std::stol;

// Constructor function
Process::Process(int pid) : pid(pid) {}

// DONE: Return this process's ID
int Process::Pid() { return this->pid;}

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
  // Number of jiffies active / total system jiffies
  return (float)LinuxParser::ActiveJiffies(this->pid) / (float)LinuxParser::Jiffies();
}

string Process::Command() { return LinuxParser::Command(this->pid); }

// DONE: Return this process's memory utilization
string Process::Ram () const { return LinuxParser::Ram(this->pid); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(this->pid);
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& b) const {
  long a_ram = stol(Ram());
  long b_ram = stol(b.Ram());
  
  // Ascending order of current RAM
  return a_ram < b_ram;
}
