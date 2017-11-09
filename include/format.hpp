#ifndef PATH_TRACKING_FORMAT_H
#define PATH_TRACKING_FORMAT_H

#include <sstream>
#include <vector>

std::string sprint(const char* format, const std::vector<std::string>& args);

namespace std
{

inline std::string to_string(const bool x) { return x ? "true" : "false"; }
inline std::string to_string(const char* x) { return x; }
inline std::string to_string(const std::string& x) { return x; }

inline std::string to_string(const auto& x)
{
	std::ostringstream ss;
	ss << x;
	return ss.str();
}

}

inline std::string format(const char* s, auto&&... args)
{
	return sprint(s, { std::to_string(args)... });
}

#endif //PATH_TRACKING_FORMAT_H
