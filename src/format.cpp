#include <string>
#include <sstream>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  string temp;
  // Create the string stream
  std::ostringstream time;
  
  // Push the hours into the stream
  temp = to_string(seconds / 3600);
  if (temp.size() == 1){
  	temp = "0" + temp;
  }
  time << temp << ":";
  
  // Chunk off the hours
  seconds = seconds % 3600;
  
  // Push the minutes in
  temp = to_string(seconds / 60);
  if (temp.size() == 1){
  	temp = "0" + temp;
  }
  time << temp << ":";
  
  // Chunk off the minutes
  seconds = seconds % 60;
  
  // Add the seconds to the stream
  temp = to_string(seconds);
  if (temp.size() == 1){
  	temp = "0" + temp;
  }
  time << temp;
  
  // Return the final value as a string
  return time.str();
}
