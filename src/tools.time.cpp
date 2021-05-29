#include "tools.hpp"


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
extern "C" char* strptime(const char* s,
                          const char* f,
                          struct tm* tm) {
  // Isn't the C++ standard lib nice? std::get_time is defined such that its
  // format parameters are the exact same as strptime. Of course, we have to
  // create a string stream first, and imbue it with the current C locale, and
  // we also have to make sure we return the right things if it fails, or
  // if it succeeds, but this is still far simpler an implementation than any
  // of the versions in any of the C standard libraries.
  std::istringstream input(s);
  input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
  input >> std::get_time(tm, f);
  if (input.fail()) {
    return nullptr;
  }
  return (char*)(s + input.tellg());
}
#endif

string tools::time::seconds_to_human_readable(double secs) 
{
    int hours=floor(secs/3600);
    int minutes=floor((secs-hours*3600)/60);
    int seconds=(secs-hours*3600-minutes*60);
    string hours_str, minutes_str,seconds_str;
    if (hours<10) hours_str="0"+to_string(hours);
    else hours_str=to_string(hours);
    if (minutes<10) minutes_str="0"+to_string(minutes);
    else minutes_str=to_string(minutes);
    if (seconds<10) seconds_str="0"+to_string(seconds);
    else seconds_str=to_string(seconds);
//     string output = to_string((float)secs/3600);
//     string output=to_string(hours)+"-"+to_string(minutes)+"-"+to_string(seconds);
    string output="";
    if (hours>0) output+=hours_str+"hours-";
    if (minutes>0) output+=minutes_str+"mins-";
    output+=seconds_str+"secs";
    return output;
}

string tools::time::tm_struct_to_string_time(tm tm, string time_format)
{
// 	string ttime;
// 	char buffer [80]="";
// 	strftime (buffer,80,time_format.c_str(),&tm);
// 	return buffer;
	stringstream stream;
	stream << std::put_time(&tm, time_format.c_str());
	return stream.str();
}

tm tools::time::string_time_to_tm_structure(string time,string time_format)
{
	struct tm tm;
	strptime(time.c_str(), time_format.c_str(), &tm);
// 	time_t T = string_to_time_t(time,time_format);
// 	struct tm * tm = std::localtime(&T);
// 	strptime(time.c_str(), time_format.c_str(), &tm);
	return tm;
}

/// http://www.cplusplus.com/reference/ctime/strftime/
time_t tools::time::string_to_time_t(string time,string time_format) 
{
// 	struct tm tm = string_time_to_tm_structure(time, time_format);
// 	std::time_t t = mktime(&tm);
	struct tm tm;
	strptime(time.c_str(), time_format.c_str(), &tm);
	time_t t = mktime(&tm);

    return t;
}

string tools::time::get_time_now(string time_format) 
{
	stringstream stream;
	auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    stream << std::put_time(&tm, time_format.c_str());
	return stream.str();
}

///http://www.cplusplus.com/reference/ctime/strftime/
string tools::time::time_t_to_string(time_t ttime, string time_format)
{
	std::ostringstream stream;
	stream << std::put_time(std::localtime(&ttime), time_format.c_str());
	string thetime = stream.str();
	stream.str("");
    stream.clear();
	return thetime;
}

///http://www.cplusplus.com/reference/ctime/strftime/
// string tools::time::tm_to_string(time_t time, string time_format)
// {
// 	std::ostringstream stream;
//     stream << std::put_time(std::localtime(&time), time_format.c_str());
// 	string time_str = stream.str();
// 	stream.str("");
//     stream.clear();
// 	return time_str;
// }
