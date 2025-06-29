#ifndef RGLZ_BASE_H
#define RGLZ_BASE_H

#if defined __linux__ || __linux || linux
	#define RGLZ_PLATFORM_LINUX
#elif defined _WIN32
	#define RGLZ_PLATFORM_WINDOWS
#endif

#include "config.h"
#include "config.def.h"

#endif	// RGLZ_BASE_H
