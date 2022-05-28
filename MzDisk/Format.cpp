#ifdef _WIN32
#pragma warning(disable:4996)
#endif

#include <stdarg.h>
#include <string>
#include <vector>

namespace dms
{
	std::string Format(const char* format, ...)
	{
		int text_buffer_size = 128;
		std::vector<char> text_buffer(text_buffer_size + 1);
		va_list arglist;
		va_start(arglist, format);
		int vssize = vsnprintf(&text_buffer[0], text_buffer_size, format, arglist);
		va_end(arglist);
		if((vssize >= 0) && (vssize < text_buffer_size))
		{
			text_buffer.resize(vssize);
			return std::string(text_buffer.begin(), text_buffer.end());
		}
#ifdef _WIN32
		va_start(arglist, format);
		vssize = _vscprintf(format, arglist);
		va_end(arglist);
#endif
		if(vssize < 0)
		{
			return "";
		}
		text_buffer.resize(vssize + 1);
		va_start(arglist, format);
		vssize = vsnprintf(&text_buffer[0], vssize + 1, format, arglist);
		va_end(arglist);
		if(vssize < 0)
		{
			return "";
		}
		text_buffer.resize(vssize);
		return std::string(text_buffer.begin(), text_buffer.end());
	}
};
