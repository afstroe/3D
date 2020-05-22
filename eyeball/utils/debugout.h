#ifndef __DEBUG_OUT_H__
#define __DEBUG_OUT_H__

#include <Windows.h>
#include <sstream>

inline void debugLogi(const char* format, std::stringstream& text) // base function
{
  text << format << "\n";
}

template<typename T, typename... Targs>
inline void debugLogi(const char* format, std::stringstream& text, T value, Targs... Fargs)
{
  for (; *format != '\0'; format++) 
  {
    if (*format == '%') 
    {
      text << value;
      debugLogi(format + 1, text, Fargs...); // recursive call
      return;
    }
    text << *format;
  }
}

template<typename T, typename... Targs>
inline void debugLog(const char* format, T value, Targs... Fargs)
{
#ifdef DOUT
  std::stringstream text;
  debugLogi(format, text, value, Fargs...);
  OutputDebugStringA(text.str().c_str());
#endif // DOUT
}
inline void debugLog(const char* format)
{
  OutputDebugStringA(format);
}

#endif // !__DEBUG_OUT_H__
