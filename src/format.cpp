#include "format.hpp"

std::string sprint(const char* format, const std::vector<std::string>& args)
{
	if (format == nullptr)
		return std::string();

	std::ostringstream ss;
	size_t idx = 0;
	while (*format)
	{
		if (*format == '{' && *(format + 1) == '}')
		{
			ss << (idx < args.size() ? args[idx++] : "{}");
			format += 2;
		}
		else
		{
			ss << *format++;
		}
	}

	return ss.str();
}
