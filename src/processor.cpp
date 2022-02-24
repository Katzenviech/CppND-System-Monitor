#include <vector>
#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 

  /*
  taken from https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  */
  auto ProcStat = LinuxParser::CpuUtilization();

  // used to store previous values, could also be implemented as private member vars, but not necessary
  static float prevuserf = 0.F;
  static float prevnicef = 0.F;
  static float prevsystemf = 0.F;
  static float previdlef = 0.F;
  static float previowaitf = 0.F;
  static float previrqf = 0.F;
  static float prevsoftirqf = 0.F;
  static float prevstealf = 0.F;
  // static float prevguestf = 0.F;
  // static float prevguest_nicef = 0.F;

  float userf, nicef, systemf, idlef, iowaitf, irqf, softirqf, stealf;
  // float guestf, guest_nicef;
  
  userf = atof(ProcStat[0].c_str());
  nicef = atof(ProcStat[1].c_str());
  systemf = atof(ProcStat[2].c_str());
  idlef = atof(ProcStat[3].c_str());
  iowaitf = atof(ProcStat[4].c_str());
  irqf = atof(ProcStat[5].c_str());
  softirqf = atof(ProcStat[6].c_str());
  stealf = atof(ProcStat[7].c_str());
  // guestf = atof(ProcStat[8].c_str());
  // guest_nicef = atof(ProcStat[9].c_str());

  
  // calculate
  float PrevIdlef = previdlef + previowaitf;
  float Idlef = idlef + iowaitf;

  float PrevNonIdlef = prevuserf + prevnicef + prevsystemf + previrqf + prevsoftirqf + prevstealf;
  float NonIdlef = userf + nicef + systemf + irqf + softirqf + stealf;

  float PrevTotalf = PrevIdlef + PrevNonIdlef;
  float Totalf = Idlef + NonIdlef;

  // differentiate: actual value minus the previous one
  float totaldf = Totalf - PrevTotalf;
  float idledf = Idlef - PrevIdlef;

  float CPU_Percentage = 0.F;
  if(totaldf > 0.F)
    CPU_Percentage = (totaldf - idledf) / totaldf;

  // save values
  prevuserf = userf;
  prevnicef = nicef;
  prevsystemf = systemf;
  previdlef = idlef;
  previowaitf = iowaitf;
  previrqf = irqf;
  prevsoftirqf = softirqf;
  prevstealf = stealf;
  // prevguestf = guestf;
  // prevguest_nicef = guest_nicef;

  return CPU_Percentage;
}