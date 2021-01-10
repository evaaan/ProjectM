#pragma once
#include <sstream>
#include <iostream>
#define odslog(msg) { std::wstringstream ss; ss << msg; OutputDebugStringW(ss.str().c_str()); std::cout << msg; }
#define odslogfatal(msg) {std::wstringstream ss; ss << msg; OutputDebugStringW(ss.str().c_str()); std::cout << msg; throw std::runtime_error(std::string("Fatal Error: ") + msg); }
