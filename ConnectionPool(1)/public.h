#pragma once
#include <iostream>

#define LOG(message)\
	std::cout << __FILE__ << ":" << __LINE__ << ":" << __TIME__ << ": "\
	<< message << std::endl;