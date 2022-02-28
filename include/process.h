#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                                   // DONE: See src/process.cpp
  std::string User();                          // DONE: See src/process.cpp
  std::string Command();                       // DONE: See src/process.cpp
  float CpuUtilization() const;                // DONE: See src/process.cpp
  std::string Ram() const;                     // DONE: See src/process.cpp
  long int UpTime();                           // DONE: See src/process.cpp
  bool operator<(const Process& other) const;  // DONE: See src/process.cpp

  // DONE: Declare any necessary private members
 private:
  int pid;

};

#endif