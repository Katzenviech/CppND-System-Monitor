#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::stringstream;


template <class T>
string to_string (const T& t)
{
    stringstream ss;
    ss << std::setw(2) << std::setfill('0') << t;
    return ss.str();
}

string Format::ElapsedTime(long seconds) { 
    long hours = seconds / 3600;
    seconds = seconds % 3600;
    long minutes = seconds / 60;
    seconds = seconds % 60;

    string hs = to_string(hours);
    string ms = to_string(minutes);
    string ss = to_string(seconds);
    
    return hs + ":" + ms + ":" + ss;
 }