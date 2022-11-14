#ifndef DEBUGMACROS
#define DEBUGMACROS

#include <iostream>
#include "espdelay.h"

#if _DEBUG

#define DEBUG_OUT(x)                std::cout << x
#define DEBUG_ERR(x)                std::cerr << x

#else

#define DEBUG_OUT(x)
#define DEBUG_ERR(x)

#endif
