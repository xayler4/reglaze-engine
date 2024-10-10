#ifndef RGLZ_BASE_H
#define RGLZ_BASE_H

#if defined __linux__ || __linux || linux
	#define RGLZ_PLATFORM_LINUX
#elif defined _WIN32
	#define RGLZ_PLATFORM_WINDOWS
#endif

#endif	// RGLZ_BASE_H
