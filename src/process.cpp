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

// Constructor
Process::Process(int pid) : pid{pid} {};

// DONE: Return this process's ID
int Process::Pid() { 
    return pid;
}

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    return LinuxParser::ActiveJiffies(pid);
}

// DONE: Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(pid);
}

// DONE: Return this process's memory utilization
string Process::Ram() const { 
    return LinuxParser::Ram(pid);
}

// DONE: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& other) const { 
    return other.CpuUtilization() < this->CpuUtilization(); 
}