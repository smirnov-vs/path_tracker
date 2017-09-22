#include "format.h"

std::string sprint(const char* s, const std::vector<std::string>& args)
{
	if (s == nullptr)
		return std::string();

	std::ostringstream ss;
	size_t idx = 0;
	while (*s)
	{
		if (*s == '{' && *(s + 1) == '}')
		{
			ss << (idx < args.size() ? args[idx++] : "{}");
			s += 2;
		}
		else
		{
			ss << *s++;
		}
	}

	return ss.str();
}
