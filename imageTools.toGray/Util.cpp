#include "Util.h"

#include <vector>

void Util::split(const std::string& str, const std::string& delim, std::vector< std::string >& result)
{
	size_t last = 0;
	auto index = str.find_first_of(delim, last);

	while (index != std::string::npos)
	{
		result.push_back(str.substr(last, index - last));
		last = index + 1;
		index = str.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		result.push_back(str.substr(last, index - last));
	}
}
