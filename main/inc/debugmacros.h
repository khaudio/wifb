#ifndef DEBUGMACROS
#define DEBUGMACROS

#include <iostream>

#if _DEBUG
#define DEBUG_OUT(x) std::cout << x;
#else
#define DEBUG_OUT(x)
#endif

#if _DEBUG
#define DEBUG_ERR(x) std::cerr << x;
#else
#define DEBUG_ERR(x)
#endif

#endif
