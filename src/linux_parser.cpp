#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <iomanip>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;

// clockticks
float CLOCKTICKS(){
  return sysconf(_SC_CLK_TCK);
}

// Helper function to read key - value from file as suggested by code-review
template <typename T>
T findValueByKey(std::string const& keyFilter, std::string const& filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

// Helper function to from file - value from file as suggested by code-review
template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

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
  struct dirent *file;
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

  float Total = 1.f; // avoid div by zero
  float Free = 0.f;

  Total = findValueByKey<float>(filterMemTotalString, kMeminfoFilename);// "/proc/memInfo"
  Free = findValueByKey<float>(filterMemFreeString, kMeminfoFilename);

  return (Total - Free) / Total;
  
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 

  long upTime = getValueOfFile<float>(kUptimeFilename);
  return upTime;

}

// DONE: Read and return the number of jiffies for the system
// taken from : https://knowledge.udacity.com/questions/129844
long LinuxParser::Jiffies() { 
  return UpTime() * CLOCKTICKS();
}
 
// Helper function to convert value to string
template<class T>
string getString(T val){
  stringstream ss;
  ss << val;
  string str = ss.str();
  return "/"+str;
}

// DONE: Read and return the number of active jiffies for a PID
/*
from https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
*/
long LinuxParser::ActiveJiffies(int pid) { 

  string val;
  vector<string> values {};
  string line;

  std::ifstream stream(kProcDirectory + getString(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    
    while(linestream >> val){
      values.push_back(val);
    }
  }

  // check if vector was filled. It can happen that the pid does not exist anymore
  if(values.size() < 22){
    return 0L;
  }

  long utime     = atol(values[13].c_str());
  long stime     = atol(values[14].c_str());
  long cutime    = atol(values[15].c_str());
  long cstime    = atol(values[16].c_str());
  long starttime = atol(values[21].c_str());
  
  long total_time = utime + stime + cutime + cstime;
  float seconds = static_cast<float>(UpTime()) - static_cast<float>(starttime / CLOCKTICKS());

  return static_cast<long>((total_time / CLOCKTICKS()) / seconds);

}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto ticks = CpuUtilization();
  long tickcount = 0;
  for(auto s : ticks){
    tickcount += atol(s.c_str());
  }
  return tickcount;
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {   
  auto ticks = CpuUtilization();
  long tickcount = atol(ticks[3].c_str()) + atol(ticks[4].c_str());

  return tickcount;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  string cpu;
  string user;
  string nice;
  string system;
  string idle;
  string iowait;
  string irq;
  string softirq;
  string steal;
  string guest;
  string guest_nice;

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  }
  
  vector<string> ProcStat {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};

  return ProcStat; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string str1;
  string str2;
  string proc_count;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> str1 >> str2;
      if(str1 == filterProcesses){
        proc_count = str2;
      }
    }
  }

  return atoi(proc_count.c_str());
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string str1;
  string str2;
  int proc_count = findValueByKey<int>(filterRunningProcesses, kStatFilename);

  return proc_count;
 }

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd = std::string(getValueOfFile<string>(getString(pid) + kCmdlineFilename));

  return cmd;
}

// DONE: Read and return the memory used by a process
/*
Change made after Code Review:
https://review.udacity.com/?utm_campaign=ret_000_auto_ndxxx_submission-reviewed&utm_source=blueshift&utm_medium=email&utm_content=trigger_enterprise_eng_3001_submission_reviewed&bsft_clkid=a10864d7-3a0a-40d6-81f2-294fa4272efb&bsft_uid=4a159d93-3acc-44b2-aab1-da5b4ed8b7a9&bsft_mid=aaf76043-dda3-4613-87ec-77c8ad8be639&bsft_eid=958a87a5-d024-4cf3-9067-8f5314206556&bsft_txnid=6a0a3600-bb17-46f0-9def-aacb8dd0a875&bsft_mime_type=html&bsft_ek=2022-02-26T10%3A41%3A23Z&bsft_aaid=17bf3774-8c65-47da-9a84-c4f6169cf022&bsft_lx=2&bsft_tv=3#!/reviews/3425099

But I should tell you that this will give you memory usage more than your Physical RAM size!

Because VmSize is the sum of all the virtual memory as you can see on the manpages also.
Search for VmSize and you will get the following line

Whereas when you use VmRSS then it gives the exact physical memory being used as a part of Physical RAM.
So it is recommended to replace the string VmSize with VmRSS as people who will be looking at your GitHub
might not have any idea of Virtual memory and so they will think you have done something wrong!
*/
string LinuxParser::Ram(int pid) { 
  string first, secnd;
  string line;
  string mem {};
  std::ifstream stream(kProcDirectory + getString(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> first >> secnd;
      if(first == filterProcMem){
        mem = secnd;
      }
    }
  }
  
  // check if successful
  if(mem.empty()){
    return "0.0";
  }

  float mem_MB = atof(mem.c_str()) / 1024.0;
  std::stringstream ss;
  ss << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(0) << mem_MB; // 02282022 - changed precision from 1 to 0
  std::string s = ss.str();
  return s;


}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string first, secnd;
  string line;
  string user_id {};
  std::ifstream stream(kProcDirectory + getString(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> first >> secnd;
      if(first == filterUID){
        user_id = secnd;
      }
    }
  }

  return user_id;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string user_id = Uid(pid);
  if(user_id.empty()) return "INVALID";
  string line;
  string name, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> name >> x >> uid){
        if(uid == user_id){
          return name;
        }
      }
    }
  }

  return "INVALID";

}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 

  string val;
  string line;
  vector<string> values {};
  std::ifstream stream(kProcDirectory + getString(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> val){
      values.push_back(val);
    }
  }
  
  if(values.size() < 22) return 0L;

  long uptime_of_pid = atol(values[21].c_str());

  return static_cast<long>(uptime_of_pid / CLOCKTICKS());

}
