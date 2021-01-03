#pragma once
#include <sstream>
#define odslog(msg) { std::wstringstream ss; ss << msg; OutputDebugStringW(ss.str().c_str()); }
#define odslogfatal(msg) {std::wstringstream ss; ss << msg; OutputDebugStringW(ss.str().c_str()); throw std::runtime_error(std::string("Fatal Error: ") + msg); }
