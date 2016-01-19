#include "util.h"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>// Sleep
	#include <direct.h> // getcwd
#else
	#include <sys/time.h>
	#include <unistd.h> // usleep, getcwd
#endif
#include <stdlib.h>   // _fullpath
#include <string.h>   // memcmp
#include <sys/stat.h> // fstat
#include <GL/glfw3.h> // glfwGetTime


	void sleep_ms(int millis)
	{
		#ifdef _WIN32
			Sleep(millis);
		#else
			usleep(1000 * millis);
		#endif
	}

	int fsize(FILE* f)
	{
		struct stat s;
		fstat(fileno(f), &s);
		return (int)s.st_size;
	}

	unsigned long long fnv64(const void* data, size_t length)
	{
		const unsigned char* p = (const unsigned char*)data;
		unsigned long long hash = 14695981039346656037ull;
		for (size_t i = 0; i < length; ++i) {
			hash ^= p[i];
			hash *= 1099511628211ull;
		}
		return hash;
	}

	char* normalize_path(char* dst, const char* relativePath)
	{
		char cwd[512]; getcwd(cwd, 512);
		char buf[512]; _fullpath(buf, relativePath, 512);
		int clen = strlen(cwd);
		char* src = memcmp(buf, cwd, clen) == 0 ? buf + clen + 1 : buf;
		return strcpy(dst, src);
	}

	char* normalized_datapath(char* dst, const char* relativePath)
	{
		memcpy(dst, "data/", 5);
		strcpy(dst+5, relativePath);
		return normalize_path(dst, dst);
	}

	const char* filepart(const char* filePath, int len)
	{
		const char* ptr = filePath;
		const char* end = filePath + len - 1;
		for (; ptr < end; --end)
			if (*end == '\\' || *end == '/')
				return end + 1;
		return ptr;
	}
	
	double timer_elapsed_vsync(double desiredFPS)
	{
		static double start = 0.0;
		if (!start) start = glfwGetTime();

		const double targetFrameTime = 1.0 / (desiredFPS + 1.0); // get vsync delay in seconds
		
		double deltaTime, time;
		while ((deltaTime = ((time = glfwGetTime()) - start)) < targetFrameTime)
			sleep_ms(1); // relinquish control back to OS

		start = time; // reset start time
		return deltaTime;
	}


