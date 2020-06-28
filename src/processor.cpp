#include "processor.h"
#include "linux_parser.h"

using std::stof;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  return (float)LinuxParser::ActiveJiffies() / (float)LinuxParser::Jiffies();
}
