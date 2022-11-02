#ifndef DEBUGMACROS
#define DEBUGMACROS

#include <iostream>

#ifdef _DEBUG
#define DEBUG_OUT(x) std::cout << x;
#else
#define DEBUG_OUT(x)
#endif

#ifdef _DEBUG
#define DEBUG_ERR(x) std::cerr << x;
#else
#define DEBUG_ERR(x)
#endif

#endif